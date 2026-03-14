#include "Parser.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    std::string stopwordPath = "stopwordlist.txt";
    std::string outputPath = "parser_output.txt";

    if (argc >= 2) stopwordPath = argv[1];
    if (argc >= 4) outputPath = argv[3];

    Parser parser;
    parser.loadStopwords(stopwordPath);

    if (argc >= 3) {
        parser.parseTrecFile(argv[2]);
    } else {
        // Default: parse all ft911 TREC files
        for (int i = 1; i <= 15; ++i) {
            std::string path = "ft911/ft911_" + std::to_string(i);
            parser.parseTrecFile(path);
        }
    }

    parser.writeOutput(outputPath);
    std::cout << "Wrote " << outputPath << " with " << parser.getWordDictionary().size()
              << " tokens and " << parser.getFileDictionary().size() << " documents.\n";
    return 0;
}
