# CS50 Lab 5
## CS50 Spring 2023

### TSE - Indexer

The indexer iterates through the output files in a Crawler directory. Scanning the html of the webpages, it creates an inverted index mapping word keys to *counter_t* structs. These structs themselves map docID's represting individual files to the number of occurences of the word in that file.

### Usage

The file `indexer.c` makes use of *hashtable*, *counters*, and *webpage* structs defined externally. `indexer.c` implements the following methods:

```c
static void indexBuild(index_t* index, char* pageDirectory);
static void indexPage(index_t* index, webpage_t* page, int docID);
```

When using `make test`, `testing.sh` expects the directories `../data/letters-1`, `../data/letters-2`, `../data/letters-3` to exist.
These directories must be readable and writeable, as well as Crawler output directories for expected functioanlity.
When using `make valgrind`, the `../data/letters-3` directory is expected to exist.

### Implementation

The indexer creates an index, which consists of a hashtable mapping words (appearing in crawled pages) to docID's (representing files in crawled directories) which themselves are keys to a count (the number of occurences of the word in that docID file).
The indexer call `indexBuild` which runs on every output file in the specified pageDirectory, calling `indexPage` on every file's html data. `indexPage` then loops through every word in the html data, creating *counter_t* structs where necessary, and then inserting into the index.

See [Implementation Docs](IMPLEMENTATION.md)

### Assumptions

No assumptions beyond those that are clear from the spec.

### Files

* `Makefile` - compilation procedure
* `indexer.c` - the implementation
* `indextest.c` - test module
* `testing.sh` - shell test file
* `testing.out` - result of `make test`
* `DESIGN.md` - design specifications
* `IMPLEMENTATION.md` - implementation specifications
* `REQUIRMENTS.md` - requirment specifications
* `.gitignore` - git ignore file

### Compilation

To compile, simply `make indexer.o`.
To use test module functionality, also `make indextest.o`.

### Testing

The `testing.sh` assumes the existence of a directories called letters-1, letters-2, and letters-3 containing the output of crawler as specified in the examples provided for Crawler.

To test, simply `make test`.
See `testing.out` for details of testing and an example test run.

To test with valgrind, `make valgrind`.
