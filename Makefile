# C++ TREC Parser for Information Retrieval
# Build: make
# Run:   ./parser [stopwordlist.txt] [trec_file] [parser_output.txt]

CXX = g++
# Suppress warnings from third-party stemmer.h (C string literals)
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -Wno-writable-strings

PARSER_TARGET = parser
PARSER_SRCS = main.cpp Parser.cpp

INDEXER_TARGET = indexer
INDEXER_SRCS = index_main.cpp Indexer.cpp

all: $(PARSER_TARGET) $(INDEXER_TARGET)

$(PARSER_TARGET): $(PARSER_SRCS) Parser.h stemmer.h
	$(CXX) $(CXXFLAGS) -o $(PARSER_TARGET) $(PARSER_SRCS)

$(INDEXER_TARGET): $(INDEXER_SRCS) Indexer.h stemmer.h
	$(CXX) $(CXXFLAGS) -o $(INDEXER_TARGET) $(INDEXER_SRCS)

clean:
	rm -f $(PARSER_TARGET) $(INDEXER_TARGET) parser_output.txt

.PHONY: all clean
