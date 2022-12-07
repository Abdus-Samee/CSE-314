#!/bin/bash

maxNum=100
maxStd=5

if [ $# -eq 1 ] && [ $1 -gt 0 ]; then
    maxNum=$1
elif [ $# -eq 2 ]; then
    if [ $1 -gt 0 ]; then
        maxNum=$1
    fi
    
    if [ $2 -ge 1 ] && [ $2 -ge 1 ] && [ $2 -le 9 ]; then
        maxStd=$2
    fi
fi

#initialize an array of size maxStd
declare -a arr

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
            arr[$i]=0
        else
            #check for file naming convention
            f=$(find -name "$i.sh")
            if [ "$f" = "" ]; then
                arr[$i]=0
            else
                bash ./$i.sh > "../../temp/$i.txt"
            fi
        fi

        cd ..
    else
        arr[$i]=0
    fi
done

cd ../temp
for file in *; do
    score=0
    d=$(diff -w "$file" ../AcceptedOutput.txt | grep ^[\>\<] | wc -l)
    score=$((maxNum-d*5))

    if [ $score -lt 0 ] ; then
        score=0
    fi

    for file2 in *; do
        if [ "$file" != "$file2" ] ; then
            d=$(diff -Z -B "$file" "$file2")
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

    arr[$(echo $file | cut -d'.' -f1)]=$score
done

for i in "${!arr[@]}"; do
    echo "$i, ${arr[$i]}" >> ../output.csv
done

cd ..
rm -rf temp
