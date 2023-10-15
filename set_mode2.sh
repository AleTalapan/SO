#!bin/bash

for linie in "$1"/*
do
   if [[ "$linie" == *.txt ]]
   then
       chmod +$2 $linie
   fi
    
   if test -d "$linie"
   then
	bash $0 $linie $2
   fi
done

