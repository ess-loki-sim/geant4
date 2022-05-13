#!/bin/bash

result=()
count=0

for D in $@/job*; do 
    if [ -d "${D}" ] && ! [ $(cat $D/exitcode.txt) == "0" ]; then
        #echo $(cat $D/exitcode.txt) == "0" 
	result+=("${D}")
	((count=count+1))
    fi
done
echo $count
echo "result is" "${result[@]}"
