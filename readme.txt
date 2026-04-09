CSCE 5200 - IR Project Phase 2
Name: Raghu Nandan Lal Garikipati
Student ID: 11754328

Build instructions
------------------
1) Open terminal in project root directory.
2) Run:
   make clean
   make

This builds:
- parser   (Phase 1 parser)
- indexer  (Phase 2 index construction + lookup)

Phase 2: Build Forward and Inverted Index
-----------------------------------------
Command format:
  ./indexer build [stopword_file] [trec_path] [output_dir]

Example used for full corpus indexing:
  ./indexer build ./stopwordlist.txt ./ft911 ./index_output

Generated output files:
- ./index_output/term_dictionary.txt
- ./index_output/doc_dictionary.txt
- ./index_output/forward_index.txt
- ./index_output/inverted_index.txt

Index file formats
------------------
Forward index:
  docID: wordID: freq; wordID: freq; ...

Inverted index:
  wordID: docID: freq; docID: freq; ...

Term lookup (used for testing)
------------------------------
Command format:
  ./indexer lookup <term> [stopword_file] [output_dir]

Example:
  ./indexer lookup work ./stopwordlist.txt ./index_output

Behavior:
- Checks whether input term is a stopword.
- If not stopword, normalizes and stems using Porter stemmer.
- Searches stem in term dictionary.
- If found, prints postings list in the format:
    stem (wordID): docID: freq; docID: freq; ...

Notes
-----
- Phase 2 is built on top of the same Phase 1 resources:
  stopword list, Porter stemmer, and TREC collection.
- Full collection indexing is supported and was used to generate
  the included forward and inverted index files.
