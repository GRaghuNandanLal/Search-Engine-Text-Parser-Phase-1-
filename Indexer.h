#ifndef INDEXER_H
#define INDEXER_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class Indexer {
public:
    using TermId = int;
    using DocId = int;

    struct IndexOutputs {
        std::string termDictionaryPath;
        std::string docDictionaryPath;
        std::string forwardIndexPath;
        std::string invertedIndexPath;
    };

    void loadStopwords(const std::string& stopwordFilePath);

    // Index a directory of TREC files (e.g., ft911/) or a single TREC file.
    // Writes dictionaries + forward/inverted indexes to outputDir.
    IndexOutputs build(const std::string& trecPath, const std::string& outputDir);

    // Lookup a raw term (will be normalized, stopword-filtered, and Porter-stemmed).
    // Reads the term dictionary + inverted index from disk and prints postings to stdout.
    // Returns 0 if term found, 1 otherwise.
    int lookupAndPrint(const std::string& rawTerm,
                       const std::string& termDictionaryPath,
                       const std::string& invertedIndexPath) const;

private:
    std::unordered_set<std::string> stopwordSet_;

    static std::string normalizeToken(const std::string& token);
    static bool containsNumericChar(const std::string& token);
    static int computeNumericId(const std::string& fullDocName);

    static std::vector<std::string> segmentDocuments(const std::string& fileContent);
    static std::string findDocumentNumber(const std::string& documentBlock);
    static std::string findDocumentText(const std::string& documentBlock);
    std::vector<std::string> tokenizeText(const std::string& rawText) const;

    static std::vector<std::string> listTrecFiles(const std::string& trecPath);

    static void ensureDirectory(const std::string& outputDir);
    static std::string joinPath(const std::string& dir, const std::string& file);

    static std::unordered_map<std::string, TermId> readTermDictionary(const std::string& path);
    static bool readInvertedIndexLineForWordId(const std::string& path, TermId wordId, std::string& outLine);
};

std::string getPorterStem(const std::string& word);

#endif
