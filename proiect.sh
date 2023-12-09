#!bin/bash

if test "$#" -ne 1
then
    echo "Incorrect number of arguments"
    exit 1
fi

count=0;
char="$1"
regex="^[A-Z][A-Za-z0-9\,\.!? ]*[\.!?]$"


while IFS= read -r line;do
    if [[ $line =~ $regex && ! $line =~ ,si && $line =~ $char ]];
    then
	(( count++ ))
    fi
    
done

echo $count


   
    
