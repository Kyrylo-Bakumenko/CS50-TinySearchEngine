#!/bin/bash
# testing.sh    Kyrylo Bakuemnko    4 May, 2023
# 
# this file tests the functionality of the TSE's indexer
# with various valid and invalid test cases

### Test indexer with various invalid arguments ###
echo -e "\n### testing with various arguments ###"

# no arguments
echo -e "\ntesting with no arguments"
./indexer

# one argument
echo -e "\ntesting with one argument"
./indexer ../data/letters-1

# three or more arguments
echo -e "\ntesting with three or more arguments"
./indexer ../data/letters-1 arg2 arg3
./indexer ../data/letters-1 arg2 arg3 arg4 arg5 a r g 6

# invalid pageDirectory
echo -e "\ntesting with invalid pageDirectory (non-existent path)"
./indexer ../data/letters-100 ../data/letters-1/index.ndx
echo -e "\ntesting with invalid pageDirectory (not a crawler directory)"
mkdir ../data/not_crawler
touch ../data/not_crawler/not_crawled.txt
./indexer ../data/not_crawler ../data/not_crawler/index.ndx
# check if extra file created
FILE=../data/not_crawler/index.ndx
if [[ -f "$FILE" ]]; then
    echo -e "\nERROR: $FILE created"
fi
# cleanup directory
rm -r ../data/not_crawler

# invalid indexFile (non-existent path)
echo -e "\ntesting with invalid indexFile (non-existent path)"
./indexer ../data/letters-1 ../data/letters-100/index.ndx

# invalid indexFile (read-only directory)
echo -e "\ntesting with invalid indexFile (read-only directory)"
chmod -w ../data/letters-1 
./indexer ../data/letters-1 ../data/letters-1/index.ndx
# reset permissions
chmod +w ../data/letters-1

# invalid indexFile (existing, read-only file)
echo -e "\ntesting with invalid indexFile (existing, read-only file)"
touch ../data/letters-1/index.ndx
chmod -w ../data/letters-1/index.ndx
./indexer ../data/letters-1 ../data/letters-1/index.ndx
# reset permissions
chmod +w ../data/letters-1/index.ndx
rm ../data/letters-1/index.ndx

### Test indexer ons everal valid pageDirectories, validated by indextest ###
echo -e "\ntesting on pageDirectory ../data/letters-1 ..."
./indexer ../data/letters-1 ../data/letters-1/index.ndx
./indextest ../data/letters-1/index.ndx ../data/letters-1/index_new.ndx
var="$(diff <(sort ../data/letters-1/index.ndx) <(sort ../data/letters-1/index_new.ndx))"
if [ -z "$var" ]
then
      echo -e "\noutput matches!"
else
      echo -e "\nOUTPUT DOES NOT MATCH"
fi
# cleanup
rm ../data/letters-1/index.ndx ../data/letters-1/index_new.ndx

echo -e "\ntesting on pageDirectory ../data/letters-2 ..."
./indexer ../data/letters-2 ../data/letters-2/index.ndx
./indextest ../data/letters-2/index.ndx ../data/letters-2/index_new.ndx
var="$(diff <(sort ../data/letters-2/index.ndx) <(sort ../data/letters-2/index_new.ndx))"
if [ -z "$var" ]
then
      echo -e "\noutput matches!"
else
      echo -e "\nOUTPUT DOES NOT MATCH"
fi
# cleanup
rm ../data/letters-2/index.ndx ../data/letters-2/index_new.ndx

echo -e "\ntesting on pageDirectory ../data/letters-3 ..."
./indexer ../data/letters-3 ../data/letters-3/index.ndx
./indextest ../data/letters-3/index.ndx ../data/letters-3/index_new.ndx
var="$(diff <(sort ../data/letters-3/index.ndx) <(sort ../data/letters-3/index_new.ndx))"
if [ -z "$var" ]
then
      echo -e "\noutput matches!"
else
      echo -e "\nOUTPUT DOES NOT MATCH"
fi
# cleanup
rm ../data/letters-3/index.ndx ../data/letters-3/index_new.ndx

### Run valgrind on both indexer and indextest to ensure no memory leaks or errors ###
echo -e "\nrunning valgrind in indexer to check for memory leaks"
valgrind --leak-check=full --show-leak-kinds=all -s ./indexer ../data/letters-1 ../data/letters-1/index.ndx

echo -e "\nrunning valgrind in indextest to check for memory leaks"
valgrind --leak-check=full --show-leak-kinds=all -s ./indextest ../data/letters-1/index.ndx ../data/letters-1/index_new.ndx

# cleanup
rm ../data/letters-1/index.ndx ../data/letters-1/index_new.ndx





