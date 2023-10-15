#!bin/bash

if test $# -lt 3
then echo "Usage: $0 file dir n1 n2 n3..."
fi

file=$1
dir=$2
shift 2
count=0
for arg
do
    if test $arg -gt 10
    then count=`expr $count + 1`
    fi
done
echo $count

sum=0
for arg
do
    sum=`expr $sum + $arg`
done
echo $sum

echo $sum | wc -L


if test -f "$file"
then echo $count $sum > "$file"
fi

IFS=""
cd $dir
for linie in *.txt
do
    cat "$linie"
    
done



