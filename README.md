# CSCE 5200 - Programming Assignment 1: Text Parser

**Information Retrieval and Web Search**

The project applies the **Text Parser** part of an IR engine: the TREC data is read, the text is tokenized, stopwords are removed, stems words (Porter algorithm) are removed and Term and Document dictionaries are constructed. Produced output is saved in parser - output.txt.

---

## Compliance with Special Instructions

This is implemented in strict compliance with the specification requirements that are given in Project Part 1:

- **Relative File Paths**: The code contains no absolute file paths (e.g.,./ft911/). There are no absolute paths and therefore it is portable.
- **Alphabetical Sorting & ID Assignment:** The unique terms will be sorted alphabetically then IDs will be assigned. Therefore, IDs are given in an absolute 1,2,3... order depending on the order of the stems in alphabet.
- **Document ID Consistency:** The name of the document (e.g. FT911-5300) is used as the suffix in the ID (e.g., 5300), and it is the same as the name of the document.

---

## Files Included


| File                | Description                                                                        |
| ------------------- | ---------------------------------------------------------------------------------- |
| `main.cpp`          | The key point which organizes the loading of the stopwords and files iteration.    |
| `Parser.cpp / .h`   | The fundamental parsing algorithms and segmentation and tokenization of documents. |
| `stemmer.h`         | The implementation of the Porter Stemming algorithm.                               |
| `stopwordlist.txt`  | The list of the common words omitted in the Term Dictionary.                       |
| `ft911/`            | Folder holding the TREC data files (ft9111-ft91115).                               |
| `Makefile`          | Script that was used to compile the parser executable.                             |
| `parser_output.txt` | The generated dictionaries are in the form of a final output file.                 |


---

## How to Build

From the project root directory (where the `Makefile` is located), execute:

```bash
make

```

Inorder to eliminate the executable and the output file created, do:

```bash
make clean

```

---

## How to Run

1. Make sure that the stopwordlist.txt and ft911/ data folder are in the same folder with the executable.
2. Execute the parser:

```bash
./parser

```

1. Program will generate or update `parser_output.txt` in the root directory.

---

## Output Format (`parser_output.txt`)

- **Term Dictionary:** `term <TAB> id`
*Example:* `aa 1`, `aaa 2`. Words are all lower case and stripped down to their Porter stem.
- **Document Dictionary:** `document_name <TAB> id`
*Example:* `FT911-1 1`. The ID is derived directly as the document name.

---

## Author

- **Name:** Raghu Nandan Lal Garikipati
- **Student ID:** 11754328

```

```

