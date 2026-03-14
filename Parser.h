#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <unordered_map>
#include <vector>

/**
 * C++ Text Parser for Information Retrieval engine.
 * Parses TREC files, tokenizes with stemming and stopword removal,
 * and builds WordDictionary and FileDictionary with token streams.
 */
class Parser {
public:
    // WordDictionary: token (stemmed) -> unique token ID
    using WordDictionary = std::unordered_map<std::string, int>;
    // FileDictionary: document name (DOCNO) -> unique doc ID
    using FileDictionary = std::unordered_map<std::string, int>;
    // Token stream for each document: list of token IDs in order
    using TokenStream = std::vector<int>;

    Parser();
    ~Parser() = default;

    /** Load stopwords from a file (one word per line). */
    void loadStopwords(const std::string& path);

    /** Parse a single TREC file; treat each <DOC>...</DOC> as one document. */
    void parseTrecFile(const std::string& filepath);

    /** Write results to parser_output.txt: tokens with IDs, then docs with IDs. */
    void writeOutput(const std::string& outputPath = "parser_output.txt") const;

    // Accessors for testing or further use
    const WordDictionary& getWordDictionary() const { return wordDict_; }
    const FileDictionary& getFileDictionary() const { return fileDict_; }
    const std::vector<TokenStream>& getDocumentTokenStreams() const { return docTokenStreams_; }

private:
    WordDictionary wordDict_;
    FileDictionary fileDict_;
    std::vector<TokenStream> docTokenStreams_;  // docTokenStreams_[docID] = token IDs

    int nextTokenId_{1};
    int nextDocId_{1};

    /** Extract text between <DOCNO> and </DOCNO> from a document block. */
    std::string extractDocno(const std::string& docBlock) const;

    /** Extract all text content from a document block (for tokenization), excluding tags. */
    std::string extractDocumentText(const std::string& docBlock) const;

    /** Split content into document blocks by <DOC>...</DOC>. */
    std::vector<std::string> splitDocuments(const std::string& content) const;

    /** Tokenize text: lowercase, remove numbers, split on non-alphanumeric, no words with digits. */
    std::vector<std::string> tokenize(const std::string& text) const;

    /** Return true if word should be filtered (stopword or empty). */
    bool isStopword(const std::string& word) const;

    /** Return true if word contains any digit (such words are ignored). */
    static bool containsDigit(const std::string& s);

    /** Get or assign token ID for a stemmed token; returns the token ID. */
    int getOrAddTokenId(const std::string& stemmedToken);

    /** Get or assign doc ID for a document name; returns the doc ID. */
    int getOrAddDocId(const std::string& docno);

    std::vector<std::string> stopwords_;
};

/** Porter stemmer: returns stemmed lowercase word (for use after lowercasing). */
std::string stemWord(const std::string& word);

#endif /* PARSER_H */
