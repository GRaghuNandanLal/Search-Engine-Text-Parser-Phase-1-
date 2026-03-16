#include "Parser.h"
#include "stemmer.h"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
#include <iostream>

std::string getPorterStem(const std::string& word) {
    if (word.empty()) return word;
    std::vector<char> stemBuffer(word.begin(), word.end());
    stemBuffer.push_back('\0');
    int endPosition = stem(stemBuffer.data(), 0, static_cast<int>(word.length()) - 1);
    return std::string(stemBuffer.data(), endPosition + 1);
}

Parser::Parser() = default;

void Parser::loadStopwords(const std::string& stopwordFilePath) {
    std::ifstream inputFile(stopwordFilePath);
    if (!inputFile.is_open()) return;
    std::string currentStopword;
    while (inputFile >> currentStopword) {
        std::transform(currentStopword.begin(), currentStopword.end(), currentStopword.begin(), ::tolower);
        stopwordSet_.insert(currentStopword);
    }
}

int Parser::computeNumericId(const std::string& fullDocName) const {
    size_t delimiterPos = fullDocName.find('-');
    if (delimiterPos != std::string::npos) {
        return std::stoi(fullDocName.substr(delimiterPos + 1));
    }
    return 0;
}

bool Parser::containsNumericChar(const std::string& token) {
    return std::any_of(token.begin(), token.end(), ::isdigit);
}

std::vector<std::string> Parser::tokenizeText(const std::string& rawText) const {
    std::vector<std::string> validTokens;
    std::string currentToken;
    for (char character : rawText) {
        if (std::isalnum(static_cast<unsigned char>(character))) {
            currentToken += static_cast<char>(std::tolower(static_cast<unsigned char>(character)));
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

void Parser::parseTrecFile(const std::string& trecFilePath) {
    std::ifstream trecFile(trecFilePath);
    if (!trecFile.is_open()) {
        std::cerr << "Error: Unable to open " << trecFilePath << std::endl;
        return;
    }
    std::string processedContent((std::istreambuf_iterator<char>(trecFile)), std::istreambuf_iterator<char>());
    
    auto documentBlocks = segmentDocuments(processedContent);
    for (const auto& block : documentBlocks) {
        std::string docIdentifier = findDocumentNumber(block);
        if (docIdentifier.empty()) continue;

        documentMap_[docIdentifier] = computeNumericId(docIdentifier);

        std::vector<std::string> tokens = tokenizeText(findDocumentText(block));
        for (const auto& t : tokens) {
            alphabeticalStems_.insert(getPorterStem(t)); 
        }
    }
}

std::vector<std::string> Parser::segmentDocuments(const std::string& fileContent) const {
    std::vector<std::string> blocks;
    std::string startTag = "<DOC>";
    std::string endTag = "</DOC>";
    size_t currentPos = 0;
    while ((currentPos = fileContent.find(startTag, currentPos)) != std::string::npos) {
        size_t endPos = fileContent.find(endTag, currentPos);
        if (endPos == std::string::npos) break;
        blocks.push_back(fileContent.substr(currentPos, endPos + endTag.length() - currentPos));
        currentPos = endPos + endTag.length();
    }
    return blocks;
}

std::string Parser::findDocumentNumber(const std::string& documentBlock) const {
    size_t start = documentBlock.find("<DOCNO>") + 7;
    size_t end = documentBlock.find("</DOCNO>");
    std::string result = documentBlock.substr(start, end - start);
    result.erase(std::remove_if(result.begin(), result.end(), ::isspace), result.end());
    return result;
}

std::string Parser::findDocumentText(const std::string& documentBlock) const {
    size_t textStart = documentBlock.find("<TEXT>");
    if (textStart == std::string::npos) return "";
    textStart += 6;
    size_t textEnd = documentBlock.find("</TEXT>");
    return documentBlock.substr(textStart, textEnd - textStart);
}

void Parser::writeOutput(const std::string& outputFileName) {
    std::ofstream outputFile(outputFileName);
    if (!outputFile.is_open()) return;
    
    int idSequence = 1;
    for (const auto& stemEntry : alphabeticalStems_) {
        termMap_[stemEntry] = idSequence;
        outputFile << stemEntry << "\t" << idSequence++ << "\n";
    }

    outputFile << "\n";

    std::vector<std::pair<std::string, int>> docPairList(documentMap_.begin(), documentMap_.end());
    std::sort(docPairList.begin(), docPairList.end(), [](const auto& firstPair, const auto& secondPair) {
        return firstPair.second < secondPair.second;
    });

    for (const auto& docPair : docPairList) {
        outputFile << docPair.first << "\t" << docPair.second << "\n";
    }
}