#!/bin/bash

cd movie_data

for file in *.txt;do
    folder=$(tail -n 2 "$file" | head -n -1)
    if [ ! -d "$folder" ];then
        mkdir "$folder"
    fi

    mv "$file" "$folder"/
done

#tail -n 2 Avatar.txt | head -n -1