#!bin/bash

cd $1
sum=0
for linie in *.txt
do
    wc -c $linie|cut -f 1 -d ' ' >> $2
    sum=`expr $sum + $(wc -c $linie|cut -f 1 -d ' ')`
done
echo "TOTAL $sum" >> $2

