#include "Parser.h"
#include <iostream>
#include <string>
#include <vector>

int main() {
    std::string stopwordFile = "./stopwordlist.txt";
    std::string finalOutputFile = "./parser_output.txt";

    Parser engineParser;
    engineParser.loadStopwords(stopwordFile);

    std::vector<std::string> dataPaths = {
        "./ft911/ft911_1", "./ft911/ft911_2", "./ft911/ft911_3",
        "./ft911/ft911_4", "./ft911/ft911_5", "./ft911/ft911_6",
        "./ft911/ft911_7", "./ft911/ft911_8", "./ft911/ft911_9",
        "./ft911/ft911_10", "./ft911/ft911_11", "./ft911/ft911_12",
        "./ft911/ft911_13", "./ft911/ft911_14", "./ft911/ft911_15"
    };

    for (const std::string& currentPath : dataPaths) {
        std::cout << "Processing: " << currentPath << "..." << std::endl;
        engineParser.parseTrecFile(currentPath);
    }

    engineParser.writeOutput(finalOutputFile);
    std::cout << "\nOperation Complete. Dictionary written to " << finalOutputFile << std::endl;

    return 0;
}