#include "Parser.h"
#include "stemmer.h"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
#include <utility>

std::string stemWord(const std::string& word) {
    if (word.empty()) return word;
    if (word.length() <= 2) return word;
    std::vector<char> buf(word.begin(), word.end());
    buf.push_back('\0');
    int endIdx = stem(buf.data(), 0, static_cast<int>(word.length()) - 1);
    return std::string(buf.data(), endIdx + 1);
}

// --- Parser implementation ---

Parser::Parser() = default;

void Parser::loadStopwords(const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open()) return;
    stopwords_.clear();
    std::string line;
    while (std::getline(f, line)) {
        // Trim leading/trailing whitespace (stopwordlist has "    a" etc.)
        auto start = line.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) continue;
        auto end = line.find_last_not_of(" \t\r\n");
        std::string w = line.substr(start, end == std::string::npos ? std::string::npos : end - start + 1);
        if (!w.empty()) {
            std::transform(w.begin(), w.end(), w.begin(), ::tolower);
            stopwords_.push_back(std::move(w));
        }
    }
}

bool Parser::containsDigit(const std::string& s) {
    return std::any_of(s.begin(), s.end(), [](unsigned char c) { return std::isdigit(c); });
}

bool Parser::isStopword(const std::string& word) const {
    return std::find(stopwords_.begin(), stopwords_.end(), word) != stopwords_.end();
}

int Parser::getOrAddTokenId(const std::string& stemmedToken) {
    auto it = wordDict_.find(stemmedToken);
    if (it != wordDict_.end()) return it->second;
    int id = nextTokenId_++;
    wordDict_[stemmedToken] = id;
    return id;
}

int Parser::getOrAddDocId(const std::string& docno) {
    auto it = fileDict_.find(docno);
    if (it != fileDict_.end()) return it->second;
    int id = nextDocId_++;
    fileDict_[docno] = id;
    return id;
}

std::vector<std::string> Parser::splitDocuments(const std::string& content) const {
    std::vector<std::string> docs;
    const std::string openTag = "<DOC>";
    const std::string closeTag = "</DOC>";
    std::string::size_type pos = 0;
    while (true) {
        auto start = content.find(openTag, pos);
        if (start == std::string::npos) break;
        auto end = content.find(closeTag, start);
        if (end == std::string::npos) break;
        end += closeTag.length();
        docs.push_back(content.substr(start, end - start));
        pos = end;
    }
    return docs;
}

std::string Parser::extractDocno(const std::string& docBlock) const {
    const std::string openTag = "<DOCNO>";
    const std::string closeTag = "</DOCNO>";
    auto start = docBlock.find(openTag);
    if (start == std::string::npos) return "";
    start += openTag.length();
    auto end = docBlock.find(closeTag, start);
    if (end == std::string::npos) return "";
    std::string docno = docBlock.substr(start, end - start);
    // Trim whitespace
    auto s = docno.find_first_not_of(" \t\r\n");
    if (s == std::string::npos) return "";
    auto e = docno.find_last_not_of(" \t\r\n");
    return docno.substr(s, e == std::string::npos ? std::string::npos : e - s + 1);
}

std::string Parser::extractDocumentText(const std::string& docBlock) const {
    std::ostringstream out;
    std::string::size_type i = 0;
    const std::size_t n = docBlock.size();
    while (i < n) {
        if (docBlock[i] == '<') {
            auto close = docBlock.find('>', i);
            if (close != std::string::npos) {
                i = close + 1;
                continue;
            }
        }
        out << docBlock[i];
        ++i;
    }
    return out.str();
}

std::vector<std::string> Parser::tokenize(const std::string& text) const {
    std::vector<std::string> tokens;
    std::string current;
    for (unsigned char c : text) {
        if (std::isalnum(static_cast<unsigned char>(c))) {
            current += static_cast<char>(std::tolower(c));
        } else {
            if (!current.empty()) {
                if (!containsDigit(current)) tokens.push_back(current);
                current.clear();
            }
        }
    }
    if (!current.empty() && !containsDigit(current)) tokens.push_back(current);
    return tokens;
}

void Parser::parseTrecFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) return;
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    std::vector<std::string> docBlocks = splitDocuments(content);

    for (const std::string& block : docBlocks) {
        std::string docno = extractDocno(block);
        if (docno.empty()) continue;

        int docId = getOrAddDocId(docno);
        // Ensure we have a token stream for this doc (docId is 1-based; we store by index docId-1)
        while (static_cast<int>(docTokenStreams_.size()) < docId) {
            docTokenStreams_.emplace_back();
        }
        TokenStream& stream = docTokenStreams_[docId - 1];

        std::string docText = extractDocumentText(block);
        std::vector<std::string> rawTokens = tokenize(docText);

        for (const std::string& token : rawTokens) {
            if (token.empty()) continue;
            if (isStopword(token)) continue;
            std::string stemmed = stemWord(token);
            if (stemmed.empty()) continue;
            int tokenId = getOrAddTokenId(stemmed);
            stream.push_back(tokenId);
        }
    }
}

void Parser::writeOutput(const std::string& outputPath) const {
    std::ofstream out(outputPath);
    if (!out.is_open()) return;

    // Build sorted view for stable output: tokens then docs
    std::vector<std::pair<std::string, int>> tokenList(wordDict_.begin(), wordDict_.end());
    std::sort(tokenList.begin(), tokenList.end(),
              [](const auto& a, const auto& b) { return a.second < b.second; });

    for (const auto& p : tokenList) {
        out << p.first << " " << p.second << "\n";
    }

    std::vector<std::pair<std::string, int>> docList(fileDict_.begin(), fileDict_.end());
    std::sort(docList.begin(), docList.end(),
              [](const auto& a, const auto& b) { return a.second < b.second; });

    for (const auto& p : docList) {
        out << p.first << " " << p.second << "\n";
    }
}
