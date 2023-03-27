#!/bin/bash

DataDir="Output/SPS_Nov2022/Original/"

Files=(
       Run001
       Run002
       Run003
       Run004
       # Run005
       # Run006
       # Run007
       # Run008
       Run009
       Run010
       Run011
       Run012
       # Run013
       # Run014       
       Run015
       Run016
       Run017
       Run018
       Run019
       # Run020
       Run021
       Run022    
      )

for i in "${Files[@]}"
do
    ./ReadVMMData_exe $i
    
    if [ ! -d $DataDir ]
    then
	mkdir -p $DataDir
    fi
    
    mv output.root $DataDir/$i.root
done
