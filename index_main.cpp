#include "Indexer.h"

#include <iostream>
#include <string>

namespace {
void printUsage(const char* prog) {
    std::cout << "Usage:\n"
              << "  Build index:\n"
              << "    " << prog << " build [stopword_file] [trec_path] [output_dir]\n\n"
              << "  Lookup term:\n"
              << "    " << prog << " lookup <term> [stopword_file] [output_dir]\n\n"
              << "Defaults:\n"
              << "  stopword_file = ./stopwordlist.txt\n"
              << "  trec_path      = ./ft911\n"
              << "  output_dir     = ./index_output\n";
}
}  // namespace

int main(int argc, char* argv[]) {
    try {
        std::string mode = (argc >= 2) ? argv[1] : "build";
        std::string stopwordFile = "./stopwordlist.txt";
        std::string trecPath = "./ft911";
        std::string outputDir = "./index_output";

        Indexer indexer;

        if (mode == "build") {
            if (argc >= 3) stopwordFile = argv[2];
            if (argc >= 4) trecPath = argv[3];
            if (argc >= 5) outputDir = argv[4];

            indexer.loadStopwords(stopwordFile);
            indexer.build(trecPath, outputDir);
            return 0;
        }

        if (mode == "lookup") {
            if (argc < 3) {
                printUsage(argv[0]);
                return 1;
            }
            const std::string queryTerm = argv[2];
            if (argc >= 4) stopwordFile = argv[3];
            if (argc >= 5) outputDir = argv[4];

            indexer.loadStopwords(stopwordFile);
            const std::string termDictionaryPath = outputDir + "/term_dictionary.txt";
            const std::string invertedIndexPath = outputDir + "/inverted_index.txt";
            return indexer.lookupAndPrint(queryTerm, termDictionaryPath, invertedIndexPath);
        }

        printUsage(argv[0]);
        return 1;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << "\n";
        return 1;
    }
}
