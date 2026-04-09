#include "Indexer.h"
#include "stemmer.h"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>

namespace fs = std::filesystem;

std::string getPorterStem(const std::string& word) {
    if (word.empty()) return word;
    std::vector<char> stemBuffer(word.begin(), word.end());
    stemBuffer.push_back('\0');
    int endPosition = stem(stemBuffer.data(), 0, static_cast<int>(word.length()) - 1);
    return std::string(stemBuffer.data(), endPosition + 1);
}

void Indexer::loadStopwords(const std::string& stopwordFilePath) {
    std::ifstream inputFile(stopwordFilePath);
    if (!inputFile.is_open()) {
        throw std::runtime_error("Unable to open stopword file: " + stopwordFilePath);
    }
    std::string currentStopword;
    while (inputFile >> currentStopword) {
        currentStopword = normalizeToken(currentStopword);
        if (!currentStopword.empty()) stopwordSet_.insert(currentStopword);
    }
}

std::string Indexer::normalizeToken(const std::string& token) {
    std::string out;
    out.reserve(token.size());
    for (unsigned char c : token) {
        if (std::isalnum(c)) out.push_back(static_cast<char>(std::tolower(c)));
    }
    return out;
}

bool Indexer::containsNumericChar(const std::string& token) {
    return std::any_of(token.begin(), token.end(), [](unsigned char c) { return std::isdigit(c); });
}

int Indexer::computeNumericId(const std::string& fullDocName) {
    size_t delimiterPos = fullDocName.find('-');
    if (delimiterPos != std::string::npos) {
        return std::stoi(fullDocName.substr(delimiterPos + 1));
    }
    return 0;
}

std::vector<std::string> Indexer::segmentDocuments(const std::string& fileContent) {
    std::vector<std::string> blocks;
    const std::string startTag = "<DOC>";
    const std::string endTag = "</DOC>";
    size_t currentPos = 0;
    while ((currentPos = fileContent.find(startTag, currentPos)) != std::string::npos) {
        size_t endPos = fileContent.find(endTag, currentPos);
        if (endPos == std::string::npos) break;
        blocks.push_back(fileContent.substr(currentPos, endPos + endTag.length() - currentPos));
        currentPos = endPos + endTag.length();
    }
    return blocks;
}

std::string Indexer::findDocumentNumber(const std::string& documentBlock) {
    size_t s = documentBlock.find("<DOCNO>");
    size_t e = documentBlock.find("</DOCNO>");
    if (s == std::string::npos || e == std::string::npos || e <= s + 7) return "";
    s += 7;
    std::string result = documentBlock.substr(s, e - s);
    result.erase(std::remove_if(result.begin(), result.end(),
                                [](unsigned char c) { return std::isspace(c); }),
                 result.end());
    return result;
}

std::string Indexer::findDocumentText(const std::string& documentBlock) {
    size_t textStart = documentBlock.find("<TEXT>");
    if (textStart == std::string::npos) return "";
    textStart += 6;
    size_t textEnd = documentBlock.find("</TEXT>", textStart);
    if (textEnd == std::string::npos) return documentBlock.substr(textStart);
    return documentBlock.substr(textStart, textEnd - textStart);
}

std::vector<std::string> Indexer::tokenizeText(const std::string& rawText) const {
    std::vector<std::string> validTokens;
    std::string currentToken;
    for (unsigned char c : rawText) {
        if (std::isalnum(c)) {
            currentToken += static_cast<char>(std::tolower(c));
        } else if (!currentToken.empty()) {
            if (!containsNumericChar(currentToken) && stopwordSet_.find(currentToken) == stopwordSet_.end()) {
                validTokens.push_back(currentToken);
            }
            currentToken.clear();
        }
    }
    if (!currentToken.empty() && !containsNumericChar(currentToken) && stopwordSet_.find(currentToken) == stopwordSet_.end()) {
        validTokens.push_back(currentToken);
    }
    return validTokens;
}

std::vector<std::string> Indexer::listTrecFiles(const std::string& trecPath) {
    std::vector<std::string> files;
    fs::path p(trecPath);
    if (!fs::exists(p)) {
        throw std::runtime_error("TREC path does not exist: " + trecPath);
    }
    if (fs::is_regular_file(p)) {
        files.push_back(p.string());
        return files;
    }
    if (!fs::is_directory(p)) {
        throw std::runtime_error("TREC path is not a file or directory: " + trecPath);
    }

    for (const auto& entry : fs::directory_iterator(p)) {
        if (entry.is_regular_file()) files.push_back(entry.path().string());
    }
    std::sort(files.begin(), files.end());
    return files;
}

void Indexer::ensureDirectory(const std::string& outputDir) {
    fs::path p(outputDir);
    if (outputDir.empty()) throw std::runtime_error("outputDir is empty");
    if (fs::exists(p)) {
        if (!fs::is_directory(p)) throw std::runtime_error("outputDir is not a directory: " + outputDir);
        return;
    }
    fs::create_directories(p);
}

std::string Indexer::joinPath(const std::string& dir, const std::string& file) {
    return (fs::path(dir) / fs::path(file)).string();
}

Indexer::IndexOutputs Indexer::build(const std::string& trecPath, const std::string& outputDir) {
    ensureDirectory(outputDir);

    const auto start = std::chrono::steady_clock::now();
    const auto trecFiles = listTrecFiles(trecPath);

    // In-memory structures
    std::set<std::string> allStemsAlphabetical;             // term strings (alphabetical)
    std::map<DocId, std::string> docIdToName;               // numeric doc id -> DOCNO
    std::map<DocId, std::unordered_map<std::string, int>> docTermFreq; // doc -> (stem -> freq)

    for (const auto& file : trecFiles) {
        std::ifstream in(file);
        if (!in.is_open()) {
            std::cerr << "Warning: unable to open " << file << "\n";
            continue;
        }
        const std::string content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
        const auto blocks = segmentDocuments(content);
        for (const auto& block : blocks) {
            const std::string docNo = findDocumentNumber(block);
            if (docNo.empty()) continue;
            const DocId docId = computeNumericId(docNo);
            if (docId == 0) continue;

            docIdToName[docId] = docNo;

            const auto tokens = tokenizeText(findDocumentText(block));
            auto& tf = docTermFreq[docId];
            for (const auto& tok : tokens) {
                const std::string stem = getPorterStem(tok);
                if (stem.empty()) continue;
                allStemsAlphabetical.insert(stem);
                ++tf[stem];
            }
        }
    }

    // Assign wordIDs based on alphabetical order of stems
    std::unordered_map<std::string, TermId> termToId;
    termToId.reserve(allStemsAlphabetical.size());
    TermId nextId = 1;
    for (const auto& stem : allStemsAlphabetical) {
        termToId.emplace(stem, nextId++);
    }

    // Build forward index: docId -> (termId -> freq)
    std::map<DocId, std::map<TermId, int>> forward;
    for (const auto& [docId, tf] : docTermFreq) {
        auto& postings = forward[docId];
        for (const auto& [stem, freq] : tf) {
            auto it = termToId.find(stem);
            if (it == termToId.end()) continue;
            postings[it->second] = freq;
        }
    }

    // Build inverted index: termId -> (docId -> freq)
    std::map<TermId, std::map<DocId, int>> inverted;
    for (const auto& [docId, termFreqs] : forward) {
        for (const auto& [termId, freq] : termFreqs) {
            inverted[termId][docId] = freq;
        }
    }

    // Write outputs
    IndexOutputs out{
        .termDictionaryPath = joinPath(outputDir, "term_dictionary.txt"),
        .docDictionaryPath = joinPath(outputDir, "doc_dictionary.txt"),
        .forwardIndexPath = joinPath(outputDir, "forward_index.txt"),
        .invertedIndexPath = joinPath(outputDir, "inverted_index.txt"),
    };

    {
        std::ofstream td(out.termDictionaryPath);
        if (!td.is_open()) throw std::runtime_error("Unable to write: " + out.termDictionaryPath);
        // write in alphabetical stem order (matches IDs)
        TermId id = 1;
        for (const auto& stem : allStemsAlphabetical) {
            td << stem << "\t" << id++ << "\n";
        }
    }
    {
        std::ofstream dd(out.docDictionaryPath);
        if (!dd.is_open()) throw std::runtime_error("Unable to write: " + out.docDictionaryPath);
        for (const auto& [docId, docNo] : docIdToName) {
            dd << docNo << "\t" << docId << "\n";
        }
    }
    {
        std::ofstream fi(out.forwardIndexPath);
        if (!fi.is_open()) throw std::runtime_error("Unable to write: " + out.forwardIndexPath);
        for (const auto& [docId, termFreqs] : forward) {
            fi << docId << ": ";
            bool first = true;
            for (const auto& [termId, freq] : termFreqs) {
                if (!first) fi << "; ";
                first = false;
                fi << termId << ": " << freq;
            }
            fi << "\n";
        }
    }
    {
        std::ofstream ii(out.invertedIndexPath);
        if (!ii.is_open()) throw std::runtime_error("Unable to write: " + out.invertedIndexPath);
        for (const auto& [termId, postings] : inverted) {
            ii << termId << ": ";
            bool first = true;
            for (const auto& [docId, freq] : postings) {
                if (!first) ii << "; ";
                first = false;
                ii << docId << ": " << freq;
            }
            ii << "\n";
        }
    }

    const auto end = std::chrono::steady_clock::now();
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::uintmax_t totalBytes = 0;
    for (const auto& p : {out.termDictionaryPath, out.docDictionaryPath, out.forwardIndexPath, out.invertedIndexPath}) {
        std::error_code ec;
        totalBytes += fs::file_size(fs::path(p), ec);
    }

    std::cout << "Indexed " << docIdToName.size() << " documents and " << allStemsAlphabetical.size()
              << " unique terms in " << ms << " ms.\n";
    std::cout << "Total index size (4 files): " << totalBytes << " bytes.\n";
    std::cout << "Wrote:\n"
              << "  " << out.termDictionaryPath << "\n"
              << "  " << out.docDictionaryPath << "\n"
              << "  " << out.forwardIndexPath << "\n"
              << "  " << out.invertedIndexPath << "\n";

    return out;
}

std::unordered_map<std::string, Indexer::TermId> Indexer::readTermDictionary(const std::string& path) {
    std::unordered_map<std::string, TermId> m;
    std::ifstream in(path);
    if (!in.is_open()) throw std::runtime_error("Unable to open term dictionary: " + path);
    std::string term;
    TermId id;
    while (in >> term >> id) {
        m.emplace(term, id);
    }
    return m;
}

bool Indexer::readInvertedIndexLineForWordId(const std::string& path, TermId wordId, std::string& outLine) {
    std::ifstream in(path);
    if (!in.is_open()) throw std::runtime_error("Unable to open inverted index: " + path);
    std::string line;
    const std::string prefix = std::to_string(wordId) + ":";
    while (std::getline(in, line)) {
        if (line.rfind(prefix, 0) == 0) { // starts with prefix
            outLine = line;
            return true;
        }
    }
    return false;
}

int Indexer::lookupAndPrint(const std::string& rawTerm,
                            const std::string& termDictionaryPath,
                            const std::string& invertedIndexPath) const {
    const std::string norm = normalizeToken(rawTerm);
    if (norm.empty() || containsNumericChar(norm)) {
        std::cout << "Invalid query term after normalization.\n";
        return 1;
    }
    if (stopwordSet_.find(norm) != stopwordSet_.end()) {
        std::cout << "'" << rawTerm << "' is a stopword.\n";
        return 1;
    }
    const std::string stem = getPorterStem(norm);
    const auto dict = readTermDictionary(termDictionaryPath);
    auto it = dict.find(stem);
    if (it == dict.end()) {
        std::cout << "Term not found in dictionary: " << stem << "\n";
        return 1;
    }
    const TermId wordId = it->second;

    std::string postingsLine;
    if (!readInvertedIndexLineForWordId(invertedIndexPath, wordId, postingsLine)) {
        std::cout << "WordID " << wordId << " not found in inverted index.\n";
        return 1;
    }

    // postingsLine format is: "wordID: docID: freq; docID: freq; ..."
    size_t colonPos = postingsLine.find(':');
    std::string postingPayload = (colonPos == std::string::npos) ? postingsLine : postingsLine.substr(colonPos + 1);
    while (!postingPayload.empty() && postingPayload.front() == ' ') postingPayload.erase(postingPayload.begin());

    // Required compact output style for grading:
    // work (123): 1: 1; 2: 1; 3: 1
    std::cout << stem << " (" << wordId << "): " << postingPayload << "\n";
    return 0;
}

