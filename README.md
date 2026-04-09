# CSCE 5200 — Information Retrieval and Web Search  
## Phase 1 (Text Parser) + Phase 2 (Indexer)

**Author:** Raghu Nandan Lal Garikipati  
**Student ID:** 11754328

---

## Project summary

This project implements two parts of an IR engine on the **same TREC document collection** (`ft911/`), using the **same stopword list** (`stopwordlist.txt`) and **Porter stemmer** (`stemmer.h`) throughout.

| Phase | Role | Executable |
|--------|------|------------|
| **Phase 1** | Parse TREC docs, tokenize, remove stopwords, stem terms, build term and document dictionaries | `./parser` |
| **Phase 2** | Build **forward** and **inverted** indexes plus word/document dictionaries; support term lookup for grading | `./indexer` |

Phase 2 is a continuation of Phase 1: preprocessing rules (stopwords, stemming, numeric-token filtering) match the Phase 1 pipeline.

---

## Requirements coverage (Phase 2)

- **Forward index:** one line per document — only terms that appear in that document, with frequencies.  
  Format: `docID: wordID: freq; wordID: freq; ...`
- **Inverted index:** one line per word — all documents where the term appears and its frequency per document.  
  Format: `wordID: docID: freq; docID: freq; ...`
- **Word dictionary:** maps each unique stem (alphabetically ordered) to a **wordID**.
- **Document dictionary:** maps each `DOCNO` (e.g. `FT911-5300`) to a numeric **docID** (suffix after `-`).
- **Query / test behavior:** For a normal English word: reject **stopwords**; otherwise **normalize**, **Porter-stem**, look up the stem in the dictionary; if present, print the **posting list** (docID and frequency per doc).  
  Lookup output format: `stem (wordID): docID: freq; docID: freq; ...`

---

## Repository layout

| Path | Description |
|------|-------------|
| `main.cpp`, `Parser.cpp`, `Parser.h` | Phase 1 parser entry and TREC parsing |
| `index_main.cpp`, `Indexer.cpp`, `Indexer.h` | Phase 2 indexing and lookup |
| `stemmer.h` | Porter stemmer |
| `stopwordlist.txt` | Stopword list |
| `ft911/` | TREC corpus (multiple segment files) |
| `Makefile` | Builds `parser` and `indexer` |
| `parser_output.txt` | Phase 1 output (generated) |
| `index_output/` | Phase 2 outputs (generated; include full-corpus `forward_index.txt` and `inverted_index.txt` for submission) |

---

## Build

From the project root (directory containing `Makefile`):

```bash
make clean
make
```

This produces:

- `parser` — Phase 1  
- `indexer` — Phase 2  

Requires a C++17 compiler (e.g. `g++`).

---

## Run — Phase 1 (parser)

Prerequisites: `stopwordlist.txt` and `ft911/` in the project root.

```bash
./parser
```

**Output:** `parser_output.txt` in the project root.

**Format (`parser_output.txt`):**

- Term lines: `stem<TAB>term_id` (stems sorted alphabetically; IDs start at 1).  
- Document lines: `DOCNO<TAB>doc_id` (doc id = numeric suffix of `DOCNO`, e.g. `FT911-1` → `1`).

---

## Run — Phase 2 (indexer)

### 1. Build indexes for the full collection (required for submission)

```bash
./indexer build [stopword_file] [trec_path] [output_dir]
```

**Recommended (full `ft911` corpus):**

```bash
./indexer build ./stopwordlist.txt ./ft911 ./index_output
```

**Outputs** (under `output_dir`, default `./index_output`):

| File | Purpose |
|------|---------|
| `term_dictionary.txt` | Word dictionary: `stem<TAB>wordID` |
| `doc_dictionary.txt` | Document dictionary: `DOCNO<TAB>docID` |
| `forward_index.txt` | Forward index (per docID) |
| `inverted_index.txt` | Inverted index (per wordID) |

The program prints indexing statistics (document count, unique term count, elapsed time, total size of the four files).

`trec_path` may be a **directory** (all regular files indexed) or a **single TREC file** (for small test collections).

### 2. Lookup a term (testing / grading)

```bash
./indexer lookup <term> [stopword_file] [output_dir]
```

**Example:**

```bash
./indexer lookup work ./stopwordlist.txt ./index_output
```

**Behavior:**

- Empty or invalid token after normalization → error message.  
- **Stopword** → message that the term is a stopword.  
- Otherwise → stem with Porter, find **wordID** in `term_dictionary.txt`, load postings from `inverted_index.txt`.

**Screen output (one line):**

```text
stem (wordID): docID: freq; docID: freq; ...
```

---

## Index file formats (exact)

**Forward index** (`forward_index.txt`):

```text
docID: wordID: freq; wordID: freq; ...
```

**Inverted index** (`inverted_index.txt`):

```text
wordID: docID: freq; docID: freq; ...
```

Only terms that occur in a document appear on that document’s forward-index line.

---

## Submission notes (Phase 2)

Typical submission includes: source code, **`forward_index.txt`** and **`inverted_index.txt`** for the **entire** Phase 1 collection, **`readme.txt`**, and **`report.pdf`**. Package as a ZIP named per course instructions (e.g. `Firstname_Lastname_IR_Phase2.zip`).  
This `README.md` mirrors the same run instructions as `readme.txt` for convenience; Canvas may ask specifically for `readme.txt`.

---

## Author

- **Name:** Raghu Nandan Lal Garikipati  
- **Student ID:** 11754328
