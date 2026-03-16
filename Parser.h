#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <unordered_map>
#include <vector>
#include <set>

class Parser {
public:
    using TermDictionary = std::unordered_map<std::string, int>;
    using DocumentDictionary = std::unordered_map<std::string, int>;

    Parser();
    ~Parser() = default;

    void loadStopwords(const std::string& stopwordFilePath);
    void parseTrecFile(const std::string& trecFilePath);
    void writeOutput(const std::string& outputFileName = "parser_output.txt");

    const TermDictionary& getTermDictionary() const { return termMap_; }
    const DocumentDictionary& getDocumentDictionary() const { return documentMap_; }

private:
    TermDictionary termMap_;
    DocumentDictionary documentMap_;
    std::set<std::string> alphabeticalStems_; 
    std::set<std::string> stopwordSet_;

    std::string findDocumentNumber(const std::string& documentBlock) const;
    std::string findDocumentText(const std::string& documentBlock) const;
    std::vector<std::string> segmentDocuments(const std::string& fileContent) const;
    std::vector<std::string> tokenizeText(const std::string& rawText) const;
    static bool containsNumericChar(const std::string& token);
    
    int computeNumericId(const std::string& fullDocName) const;
};

std::string getPorterStem(const std::string& word);

#endif