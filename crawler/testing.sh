#!/bin/bash
# testing.sh    Kyrylo Bakuemnko    28 April, 2023
# 
# this file tests the functionality of the TSE's crawler
# with various valid and invalid test cases


## Invalid Use Cases ##
# missing parameters
./crawler
# missing parameters
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html
# extra parameters
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/letters 2 EXTRA
# invalid parameters
./crawler arg rga gar
# invalid parameters
./crawler 123 321 213

## Valid Use Cases ##
# create directory if non-existant
mkdir -p ../data/letters-10
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/letters-10 10

max=1
# letters
echo "comparing letters output . . ."
for i in `seq 0 $max`
do
    echo "running letters-${i}"
    # create directory if non-existant
    mkdir -p ../data/letters-"${i}"
    ./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/letters-"${i}" $i
    # Compare with expected output ## ONLY PERFORMED LOCALLY ##
    # echo "Difference in directories data-correct (expected) and data (output)"
    # echo "[Should be EMPTY]"
    # diff -r ../data-correct/letters-"${i}" ../data/letters-"${i}"
done
## test on larger linked websites ##
# toscrape
echo "comparing toscrape output . . ."
for i in `seq 0 $max`
do
    echo "running toscrape-${i}"
    # create directory if non-existant
    mkdir -p ../data/toscrape-"${i}"
    ./crawler http://cs50tse.cs.dartmouth.edu/tse/toscrape/index.html ../data/toscrape-"${i}" $i
    diff -r ../data-correct/toscrape-"${i}" ../data/toscrape-"${i}"
    # Compare with expected output ## ONLY PERFORMED LOCALLY ##
    # echo "Difference in directories data-correct (expected) and data (output)"
    # echo "[Should be EMPTY]"
    # diff -r ../data-correct/toscrape-"${i}" ../data/toscrape-"${i}"
done
# wiki
echo "comparing wiki output . . ."
for i in `seq 0 $max`
do
    echo "running wikipedia_${i}"
    # create directory if non-existant
    mkdir -p ../data/wikipedia_"${i}"
    ./crawler http://cs50tse.cs.dartmouth.edu/tse/wikipedia/ ../data/wikipedia_"${i}" $i
    # Compare with expected output ## ONLY PERFORMED LOCALLY ##
    # echo "Difference in directories data-correct (expected) and data (output)"
    # echo "[Should be EMPTY]"
    # diff -r ../data-correct/wikipedia_"${i}" ../data/wikipedia_"${i}"
done

exit 0


