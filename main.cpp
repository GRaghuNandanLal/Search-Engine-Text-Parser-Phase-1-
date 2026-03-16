#include "Parser.h"
#include <iostream>
#include <string>
#include <vector>

int main() {
    // Required: use relative paths only (no absolute paths like C:/... or /Users/...)
    std::string stopwordPath = "./stopwordlist.txt";
    std::string outputPath = "./parser_output.txt";

    Parser parser;
    parser.loadStopwords(stopwordPath);

    // Relative paths: ./ft911/ft911_N
    std::vector<std::string> dataFiles = {
        "./ft911/ft911_1",
        "./ft911/ft911_2",
        "./ft911/ft911_3",
        "./ft911/ft911_4",
        "./ft911/ft911_5",
        "./ft911/ft911_6",
        "./ft911/ft911_7",
        "./ft911/ft911_8",
        "./ft911/ft911_9",
        "./ft911/ft911_10",
        "./ft911/ft911_11",
        "./ft911/ft911_12",
        "./ft911/ft911_13",
        "./ft911/ft911_14",
        "./ft911/ft911_15",
    };

    for (const std::string& path : dataFiles) {
        std::cout << "Attempting to open: " << path << "..." << std::endl;
        parser.parseTrecFile(path);
    }

    parser.writeOutput(outputPath);
    std::cout << "\nSuccess! Results written to " << outputPath << std::endl;

    return 0;
}