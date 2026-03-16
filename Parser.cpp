#include "Parser.h"
#include "stemmer.h"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
#include <iostream>

std::string stemWord(const std::string& word) {
    if (word.empty()) return word;
    std::vector<char> buf(word.begin(), word.end());
    buf.push_back('\0');
    int endIdx = stem(buf.data(), 0, static_cast<int>(word.length()) - 1);
    return std::string(buf.data(), endIdx + 1);
}

Parser::Parser() = default;

void Parser::loadStopwords(const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open()) return;
    std::string w;
    while (f >> w) {
        std::transform(w.begin(), w.end(), w.begin(), ::tolower);
        stopwords_.insert(w);
    }
}

int Parser::parseNumericId(const std::string& docno) const {
    size_t dash = docno.find('-');
    if (dash != std::string::npos) {
        return std::stoi(docno.substr(dash + 1));
    }
    return 0;
}

bool Parser::containsDigit(const std::string& s) {
    return std::any_of(s.begin(), s.end(), ::isdigit);
}

std::vector<std::string> Parser::tokenize(const std::string& text) const {
    std::vector<std::string> tokens;
    std::string current;
    for (char ch : text) {
        if (std::isalnum(static_cast<unsigned char>(ch))) {
            current += static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
        } else if (!current.empty()) {
            if (!containsDigit(current) && stopwords_.find(current) == stopwords_.end()) {
                tokens.push_back(current);
            }
            current.clear();
        }
    }
    if (!current.empty() && !containsDigit(current) && stopwords_.find(current) == stopwords_.end()) {
        tokens.push_back(current);
    }
    return tokens;
}

void Parser::parseTrecFile(const std::string& filepath) {
    std::ifstream f(filepath);
    if (!f.is_open()) {
        std::cerr << "Could not open: " << filepath << std::endl;
        return;
    }
    std::string content((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    
    auto blocks = splitDocuments(content);
    for (const auto& block : blocks) {
        std::string docno = extractDocno(block);
        if (docno.empty()) continue;

        // Requirement 3: Document ID must match X in FT911-X (e.g. FT911-3546 -> ID 3546)
        fileDict_[docno] = parseNumericId(docno);

        std::vector<std::string> rawTokens = tokenize(extractDocumentText(block));
        for (const auto& t : rawTokens) {
            uniqueStems_.insert(stemWord(t)); // Collect unique stems
        }
    }
}

std::vector<std::string> Parser::splitDocuments(const std::string& content) const {
    std::vector<std::string> blocks;
    std::string startTag = "<DOC>";
    std::string endTag = "</DOC>";
    size_t pos = 0;
    while ((pos = content.find(startTag, pos)) != std::string::npos) {
        size_t end = content.find(endTag, pos);
        if (end == std::string::npos) break;
        blocks.push_back(content.substr(pos, end + endTag.length() - pos));
        pos = end + endTag.length();
    }
    return blocks;
}

std::string Parser::extractDocno(const std::string& block) const {
    size_t start = block.find("<DOCNO>") + 7;
    size_t end = block.find("</DOCNO>");
    std::string res = block.substr(start, end - start);
    res.erase(std::remove_if(res.begin(), res.end(), ::isspace), res.end());
    return res;
}

std::string Parser::extractDocumentText(const std::string& block) const {
    size_t start = block.find("<TEXT>");
    if (start == std::string::npos) return "";
    start += 6;
    size_t end = block.find("</TEXT>");
    return block.substr(start, end - start);
}

void Parser::writeOutput(const std::string& outputPath) {
    std::ofstream out(outputPath);
    
    // Requirement 2: Words sorted alphabetically first, then IDs assigned in order (1, 2, 3, ...)
    int id = 1;
    for (const auto& stem : uniqueStems_) {
        wordDict_[stem] = id;
        out << stem << "\t" << id++ << "\n";
    }

    out << "\n";

    // Requirement 3: Document ID = number in document name (FT911-X -> ID = X)
    std::vector<std::pair<std::string, int>> docList(fileDict_.begin(), fileDict_.end());
    std::sort(docList.begin(), docList.end(), [](const auto& a, const auto& b) {
        return a.second < b.second; // Sort by ID for clean output
    });

    for (const auto& p : docList) {
        out << p.first << "\t" << p.second << "\n";
    }
}