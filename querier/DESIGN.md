# CS50 TSE Querier
## Design Spec

In this document we reference the [Requirements Specification](REQUIREMENTS.md) and focus on the implementation-independent design decisions.
The knowledge unit noted that an [design spec](https://github.com/CS50spring2023/home/blob/main/knowledge/units/design.md#design-spec) may include many topics; not all are relevant to the TSE or the Querier.
Here we focus on the core subset:

- User interface
- Inputs and outputs
- Functional decomposition into modules
- Pseudo code (plain English-like language) for logic/algorithmic flow
- Major data structures
- Testing plan

## User interface

As described in the [Requirements Spec](REQUIREMENTS.md), the querier's only interface with the user is on the command-line; it must always have two arguments.

```bash
$ ./querier pageDirectory indexFilename
```

For example, to query on a page directory output by `crawler.c` and an index file indexed by `indexer.c`, store the files in a subdirectory `data` in the current directory. Here is an example of the querier been evoked:

``` bash
$ ./querier ../data/letters-10 ../data/letters-10/index.ndx
```

To prepare the necessary files, one may evoke `crawler.c` and `indexer.c`

``` bash
$ mkdir ../data/letters-10
$ ./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/letters-10 10
$ ./indexer ../data/letters-10 ../data/letters-10/index.ndx
```

### Inputs and outputs

**Input**: The querier reads input from stdin either through tty or from an input file with a query for every line.

**Output**: The only output of querier is the cleaned query and a list of docs ranked by score as described in `REQUIRMENTS.md`. This is send to stdout.

### Functional decomposition into modules

We anticipate the following modules or functions:

 1. *main*, which parses arguments and initializes other modules;
 2. *indexBuild*, which builds an in-memory index from webpage files it finds in the pageDirectory;
 2. *indexPage*, which scans a webpage document to add its words to the index.

And some helper modules that provide data structures or additional functionalities:

 1. *index*, a module providing the data structure to represent the in-memory index, and functions to read index files and find the length of a directory;
 1. *word*, a module providing the data structure to represent webpages, and to scan a webpage for words;
 2. *file*, a module providing functions to read from stdin and files;
 4. *mem*, a module providing a wrapper functions for `malloc` and `free`.

### Pseudo code for logic/algorithmic flow

The querier will run as follows:

    parse the command line, validate parameters, initialize other modules
    call query
    cleanup structures from other modules

where *query:*

    loops through stdin input
    verifies input with parse_query
    if valid:
        calls index_search, page_rank

where *parse_query:*

    normalizes query input
    tokenizes input with parse_words
    verifies input with verify_query

where *verify_query:*

    loops through every character:
        verifies all are in accordance with IMPLEMENTATION.md
    loops through every word:
        verifies all are in accordance with IMPLEMENTATION.md

where *parse_words:*

    tokenizes input of query into words by whitespace

### Major data structures

The key data structure used is the *index*, mapping from *word* to *(docID, #occurrences)* pairs.
The *index* is a *hashtable* keyed by *word* and storing *counters* as items.
The *counters* is keyed by *docID* and stores a count of the number of occurrences of that word in the document with that ID. 

### Testing plan

*Integration testing*.  The *indexer*, as a complete program, will be tested by building an index from a pageDirectory, and then the resulting index will be validated by running it through the *indextest* to ensure it can be loaded.

1. Test `querier` with various invalid arguments.
	2. no arguments
	3. one argument
	4. three or more arguments
	5. invalid `pageDirectory` (non-existent path)
	5. invalid `pageDirectory` (not a crawler directory)
	6. invalid `indexFile` (non-existent path)
0. Run *querier* on a variety of pageDirectories and comapre with manually checked output files to validate results.
0. Run *valgrind* on *querier* to ensure no memory leaks or errors.