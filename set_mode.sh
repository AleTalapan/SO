#!bin/bash

cd $1
for linie in *.txt
do
    chmod +$2 $linie
done

	    
