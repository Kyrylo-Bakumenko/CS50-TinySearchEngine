#!/bin/bash
# testing.sh    Kyrylo Bakuemnko    12 May, 2023
# 
# this file tests the functionality of the TSE's querier
# with various valid and invalid test cases

### Test indexer with various invalid arguments ###
echo -e "\n### testing with various arguments ###"

# no arguments
echo -e "\ntesting with no arguments"
./querier

# one argument
echo -e "\ntesting with one argument"
./querier ../data/letters-10

# three or more arguments
echo -e "\ntesting with three or more arguments"
./querier ../data/letters-10 arg2 arg3
./querier ../data/letters-10 arg2 arg3 arg4 arg5 a r g 6

# invalid pageDirectory
echo -e "\ntesting with invalid pageDirectory (non-existent path)"
./querier ../data/letters-100 ../data/letters-1/index.ndx

# pageDirectory not a crawler directory
echo -e "\ntesting with invalid pageDirectory (not a crawler directory)"
mkdir ../data/not_crawler
touch ../data/not_crawler/not_crawled.txt
./querier ../data/not_crawler ../data/not_crawler/index.ndx
# check if extra file created
FILE=../data/not_crawler/index.ndx
if [[ -f "$FILE" ]]; then
    echo -e "\nERROR: $FILE created"
fi
# cleanup directory
rm -r ../data/not_crawler

# invalid indexFile (non-existent path)
echo -e "\ntesting with invalid indexFile (non-existent path)"
./querier ../data/letters-10 ../data/letters-100/index.ndx

### Test indexer on several valid pageDirectories, validated by previously verified run by hand ###
pdir="../data/letters-10"
indx="../data/letters-10/index.ndx"
# test1_correct.out was manually verified to contian correct behaviour
echo -e "\ntesting on pageDirectory: $pdir"
touch test1.out
./querier $pdir $indx < test1 >& test1.out
var="$(diff test1.out test1_correct.out)"
if [ -z "$var" ]
then
      echo -e "\noutput matches!"
else
      echo -e "\nOUTPUT DOES NOT MATCH"
fi
# cleanup files
rm test1.out

# test2_correct.out was manually verified to contian correct behaviour
echo -e "\ntesting on pageDirectory: $pdir with fuzzyquery"
touch test2.out
./fuzzquery $indx 10 0 | ./querier $pdir $indx >& test2.out
var="$(diff test2.out test2_correct.out)"
if [ -z "$var" ]
then
      echo -e "\noutput matches!"
else
      echo -e "\nOUTPUT DOES NOT MATCH"
fi
# cleanup files
rm test2.out

pdir="../data/toscrape-2"
indx="../data/toscrape-2/index.ndx"
# test3_correct.out was manually verified to contian correct behaviour
echo -e "\ntesting on pageDirectory: $pdir with fuzzyquery"
touch test3.out
./fuzzquery $indx 10 0 | ./querier $pdir $indx >& test3.out
var="$(diff test3.out test3_correct.out)"
if [ -z "$var" ]
then
      echo -e "\noutput matches!"
else
      echo -e "\nOUTPUT DOES NOT MATCH"
fi
# cleanup files
rm test3.out

# ### Run valgrind on both indexer and indextest to ensure no memory leaks or errors ###
echo -e "\nrunning valgrind in indexer to check for memory leaks"
valgrind --leak-check=full --show-leak-kinds=all -s ./querier ../data/letters-10 ../data/letters-10/index.ndx < test1




