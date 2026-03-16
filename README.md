# CSCE 5200 - Programming Assignment 1: Text Parser

**Information Retrieval and Web Search**

This project implements the **Text Parser** component of an IR engine: it reads TREC data, tokenizes text, removes stopwords, stems words (Porter algorithm), and builds Term and Document dictionaries. Output is written to `parser_output.txt`.

---

## Files Included

| File | Description |
|------|-------------|
| `main.cpp` | Entry point; loads stopwords, parses TREC files, writes output. |
| `Parser.cpp`, `Parser.h` | Parser logic: tokenization, stemming, document splitting, dictionary building. |
| `stemmer.h` | Porter Stemming algorithm implementation. |
| `stopwordlist.txt` | Stopwords to exclude from the term dictionary. |
| `ft911/` | Directory containing TREC data files (`ft911_1`, `ft911_2`, …). |
| `Makefile` | Builds the `parser` executable. |
| `parser_output.txt` | Generated output (term IDs and document IDs). |

---

## Prerequisites

- **Compiler:** `g++` with C++17 (e.g. GCC 7+, Clang 5+).
- **OS:** Linux, macOS, or Windows (MinGW/WSL).

---

## How to Build

From the **project root directory** (where `Makefile`, `main.cpp`, and `Parser.cpp` are located):

```bash
make
```

This produces the executable `parser`.

To clean build artifacts and the generated output:

```bash
make clean
```

---

## How to Run

1. **Run from the project root directory** so that relative paths (`./ft911/`, `./stopwordlist.txt`, `./parser_output.txt`) resolve correctly.

2. Ensure these exist before running:
   - `./stopwordlist.txt`
   - `./ft911/` with the TREC data files (e.g. `ft911_1`, `ft911_2`, …)

3. Execute:

```bash
./parser
```

4. The program will:
   - Load stopwords from `./stopwordlist.txt`
   - Parse each TREC file listed in `main.cpp` (e.g. `./ft911/ft911_1` … `./ft911/ft911_15`)
   - Write **parser_output.txt** in the project root with:
     - **Term dictionary:** unique stemmed terms in alphabetical order with IDs 1, 2, 3, …
     - **Document dictionary:** document names (e.g. FT911-X) with ID = X

---

## Output Format (`parser_output.txt`)

- **Part 1 – Term dictionary:**  
  `term<TAB>id`  
  Terms are sorted alphabetically; IDs are assigned in that order.

- **Part 2 – Document dictionary:**  
  `document_name<TAB>id`  
  For documents of the form `FT911-X`, the ID is the number X (e.g. FT911-3546 → 3546).

---

## Changing Input Files

To use different or additional TREC files, edit the `dataFiles` vector in `main.cpp` and keep paths **relative** (e.g. `"./ft911/ft911_1"`). Do not use absolute paths.

---

## Author

Raghu Nandan Lal Garikipati  
CSCE 5200 - Information Retrieval
