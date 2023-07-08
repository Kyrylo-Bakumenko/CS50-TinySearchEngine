# CS50 TSE
## Kyrylo Bakumenko (Kyrylo-Bakumenko)

### TSE - Tiny Search Engine

This project implements the Tiny Search Enginer per CS50 specifications.
The Tiny Search Engine accpets an internal url as a starting point, a max depth until which
a DFS search for internal url's is performed, and an existing directory to which to write files.

Written files will contain URL, depth, and HTML information and saved with a file name represting the order
in which they were found by `crawl/crawl.c`.

These files are then parsed by `indexer/indexer.c`. This file creates an index storing the number of times a word appeared in a particular file, then writes the inverted index to a new file in the same directory (indexFilename at discretion of user, see `indexer/README.md`).

Finally, the specified pageDirectory directory and indexFilename file are read by `querier/querier.c`, which in turn accept queries from stdin provided through file or tty. The output is a cleaned query to stdout as well as a list of crawled pages from pageDirectory, ranked by score (relevance) as described in `querier/IMPLEMENTATION.md`.

### Assumptions

No assumptions beyond those that are clear from the spec.

### Usage

make and clean functionality can be evoked from this TSE directory.
However, make test and make valgrind should be evoked from the `/crawler`, `/indexer`, `/querier` directories respetively.

Crawler output from testing is in `/crawler/testing.out`
Indexer output from testing is in `/indexer/testing.out`
Querier output from testing is in `/querier/testing.out`

