#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <unordered_map>
#include <vector>
#include <set>

class Parser {
public:
    using WordDictionary = std::unordered_map<std::string, int>;
    using FileDictionary = std::unordered_map<std::string, int>;
    using TokenStream = std::vector<int>;

    Parser();
    ~Parser() = default;

    void loadStopwords(const std::string& path);
    void parseTrecFile(const std::string& filepath);
    void writeOutput(const std::string& outputPath = "parser_output.txt");

    const WordDictionary& getWordDictionary() const { return wordDict_; }
    const FileDictionary& getFileDictionary() const { return fileDict_; }

private:
    WordDictionary wordDict_;
    FileDictionary fileDict_;
    std::set<std::string> uniqueStems_; // NEW: Ensures alphabetical sorting
    std::set<std::string> stopwords_;    // Changed to set for O(log n) lookup

    std::string extractDocno(const std::string& docBlock) const;
    std::string extractDocumentText(const std::string& docBlock) const;
    std::vector<std::string> splitDocuments(const std::string& content) const;
    std::vector<std::string> tokenize(const std::string& text) const;
    static bool containsDigit(const std::string& s);
    
    // Helper to extract X from FT911-X
    int parseNumericId(const std::string& docno) const;
};

std::string stemWord(const std::string& word);

#endif