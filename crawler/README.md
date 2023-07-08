# CS50 Lab 4
## CS50 Spring 2023

### TSE - Crawler

The TSE crawler recursively searches for internal links from a seed url in a depth first search until a predifined depth passed as an argument. The pages linked are then added in order found, excluding duplicates, to a predifined directory passed as an argument.

### Usage

The file `crawler.c` makes use of *hashtable* and *bag* structs defined externally. `crawler.c` implements the following methods:

```c
static void parseArgs(const int argc, char* argv[], char** seedURL, char** pageDirectory, int* maxDepth);
static void crawl(char* seedURL, char* pageDirectory, const int maxDepth);
static void pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen);
static void logr(const char* word, const int depth, const char* url);
```

When using `make test`, if desired directories are non-existant they will be created upon evocation of `make test`.
When using `make valgrind`, if the desired directorie is non-existant it will be created upon evocation of `make valgrind`.

### Implementation

A depth first search is performed using a passed valid internal url as the starting point. The DFS search is carried out until maxDepth,
passed as a parameter is reached. All found non-duplicate links are added to a specified directory. The files are writen with a docID which
is the order in which they were found and the written file contains the URL, depth, and HTML information for any found page.

This file makes use of the *webpage* struct implemented externally in `webpage.c`.

### Assumptions

No assumptions beyond those that are clear from the spec.

### Files

* `Makefile` - compilation procedure
* `crawler.c` - the implementation
* `testing.sh` - shell test file
* `testing.out` - result of `make test`
* `DESIGN.md` - design specifications
* `IMPLEMENTATION.md` - design specifications
* `REQUIRMENTS.md` - requirment specifications
* `.gitignore` - git ignore file

### Compilation

To compile, simply `make crawler.o`.

### Testing

The `testing.sh` has hardcoded testcases passing invalid and valid arguments of different seed url's, depth, and page directories.

To test, simply `make test`.
See `testing.out` for details of testing and an example test run.

To test with valgrind, `make valgrind`.
