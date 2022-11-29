#!/bin/bash

maxNum=100
maxStd=5

if [ $# -eq 1 ] ; then
    maxNum=$1
elif [ $# -eq 2 ] ; then
    maxNum=$1
    
    if [ $2 -ge 1 ] && [ $2 -le 9 ] ; then
        maxStd=$2
    fi
fi

maxStd=$((maxStd+1805120))

cd ..
echo "student_id, score" > output.csv
mkdir temp
cd Submissions

for(( i=1805121; i<=maxStd; i++ ));do
    #check if directory named i is present or not
    if [ -d $i ]; then
        cd $i

        #file is not present and file naming convention check
        if [ ! -f $i.sh ]; then
            echo $i, 0 >> ../../output.csv
        else
            #check for file naming convention
            f=$(find -name "$i.sh")
            if [ "$f" = "" ]; then
                echo $i, 0 >> ../../output.csv
            else
                bash ./$i.sh > "../../temp/$i.txt"
            fi
        fi

        cd ..
    else
        echo $i, 0 >> ../../output.csv
    fi
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
