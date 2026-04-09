# CSCE 5200 - Information Retrieval
## Project Phase 2 Report: Search Engine Index Construction

**Student Name:** Raghu Nandan Lal Garikipati  
**Student ID:** 11754328

## 1. Overview

Project Phase 2 extends the Phase 1 parser to construct indexing structures for retrieval.  
The implementation uses the same resources from Phase 1:
- TREC collection (`ft911`)
- stopword list (`stopwordlist.txt`)
- Porter stemmer (`stemmer.h`)

The system builds:
1. Word Dictionary (term -> wordID)
2. Document Dictionary (DOCNO -> docID)
3. Forward Index (docID -> list of wordIDs with frequencies)
4. Inverted Index (wordID -> list of docIDs with frequencies)

## 2. Design and Implementation

The indexer is implemented in C++ (`Indexer.cpp/.h`, `index_main.cpp`) and follows this pipeline:

1. **Document parsing**
   - Read each file in the corpus path.
   - Split into `<DOC> ... </DOC>` blocks.
   - Extract `<DOCNO>` and `<TEXT>` content.

2. **Text preprocessing**
   - Convert to lowercase.
   - Tokenize by alphanumeric characters.
   - Remove stopwords.
   - Remove tokens containing numeric characters.
   - Apply Porter stemming.

3. **Dictionary construction**
   - Collect all unique stems and sort alphabetically.
   - Assign wordIDs in ascending order starting from 1.
   - Compute numeric docIDs from DOCNO suffix (e.g., `FT911-5300` -> `5300`).

4. **Index construction**
   - Build term frequency map per document.
   - Build **forward index** from doc term frequencies.
   - Build **inverted index** by transposing the forward index.

## 3. Output Files and Formats

All files are generated in `index_output/`:
- `term_dictionary.txt`
- `doc_dictionary.txt`
- `forward_index.txt`
- `inverted_index.txt`

Formats used:
- **Forward index:**  
  `docID: wordID: freq; wordID: freq; ...`
- **Inverted index:**  
  `wordID: docID: freq; docID: freq; ...`

Only terms that occur in a document are stored for that document entry.

## 4. Query Term Lookup Interface

The program supports:
`./indexer lookup <term> [stopword_file] [output_dir]`

Processing steps for input term:
1. Normalize term.
2. Check stopword list.
3. Stem using Porter stemmer.
4. Search stem in word dictionary.
5. Retrieve posting list from inverted index.

Output format:
`stem (wordID): docID: freq; docID: freq; ...`

## 5. Runtime and Index Size (Full Corpus)

Command used:
`./indexer build ./stopwordlist.txt ./ft911 ./index_output`

Measured result (latest run):
- Number of documents indexed: **5368**
- Number of unique terms indexed: **32645**
- Indexing time: **615 ms**
- Total index size (all 4 files): **13,144,765 bytes**

## 6. Conclusion

The Phase 2 implementation successfully builds forward and inverted indexes for the complete dataset used in Phase 1, while reusing the same stopword removal and Porter stemming pipeline.  
It also provides a query lookup interface that returns posting-list information in the required format for testing and evaluation.
