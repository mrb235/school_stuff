#!/bin/bash

if [[ -f $2 ]]
then
    echo "false worked"
else
    while read line
    do
        name=$line

        num=0
        #from http://stackoverflow.com/questions/10586153/bash-split-string-into-array
        IFS='	' read -a array <<< "$name"
        for s in ${array[@]}
        do
            num=$(($num + $s))
        done
        echo $(($((${#array[@]}+1))/2))
        echo $num

    done < $2
fi
