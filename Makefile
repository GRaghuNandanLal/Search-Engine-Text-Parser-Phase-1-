# C++ TREC Parser for Information Retrieval
# Build: make
# Run:   ./parser [stopwordlist.txt] [trec_file] [parser_output.txt]

CXX = g++
# Suppress warnings from third-party stemmer.h (C string literals)
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -Wno-writable-strings
TARGET = parser
SRCS = main.cpp Parser.cpp

$(TARGET): $(SRCS) Parser.h stemmer.h
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET) parser_output.txt

.PHONY: clean
