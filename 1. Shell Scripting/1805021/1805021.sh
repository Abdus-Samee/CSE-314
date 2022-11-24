#!/bin/bash

maxNum=$#

if [ $maxNum -lt 1 ] ; then
    maxNum=100
fi

cd ..
mkdir temp
cd Submissions

for file in *; do
    cd $file
    bash ./$file.sh > "../../temp/$file.txt"
    cd ..
done

cd ../temp
for file in *; do
    diff -b "$file" "../AcceptedOutput.txt"
done 

cd ..
rm -rf temp