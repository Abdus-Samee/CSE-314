#!/bin/bash

maxNum=$1

if [ $# -lt 1 ] ; then
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
    score=0
    #d=$(diff -w -y --suppress-common-lines "$file" ../AcceptedOutput.txt | wc -l)
    d=$(diff -w "$file" ../AcceptedOutput.txt | grep ^[\>\<] | wc -l)
    score=$((maxNum-d*5))

    if [ $score -lt 0 ] ; then
        score=0
    fi

    #find diff of file with other files
    for file2 in *; do
        if [ "$file" != "$file2" ] ; then
            d=$(diff "$file" "$file2")
            if [ "$d" = "" ] ; then
                if [ $score -gt 0 ] ; then
                    score=$((-1*score))
                    break
                elif [ $score -eq 0 ] ; then
                    score=-100
                    break
                fi
            fi
        fi
    done

    echo "$file: $score"
done 

cd ..
rm -rf temp
