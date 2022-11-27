#!/bin/bash

maxNum=$1

if [ $# -lt 1 ] ; then
    maxNum=100
fi

cd ..
echo "student_id, score" > output.csv
mkdir temp
cd Submissions

for file in *; do
    cd $file

    #file is not present and file naming convention check
    if [ ! -f $file.sh ]; then
        #echo "$file.txt: $score"
        echo $file, 0 >> ../../output.csv
    else
        #check for file naming convention
        f=$(find -name "$file.sh")
        if [ "$f" = "" ]; then
            #echo "$file.txt: $score"
            echo $file, 0 >> ../../output.csv
        else
            bash ./$file.sh > "../../temp/$file.txt"
        fi
    fi

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

    #echo "$file: $score"
    echo $(echo $file | cut -d'.' -f1), $score >> ../output.csv
    #name=$(cut -f 1 -d '.' "$file")
    #echo "$name, $score" >> ../output.csv
done 

cd ..
rm -rf temp
