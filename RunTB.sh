#!/bin/bash


#######################################################
#
#  This script can be used to run the VMM Analyser on
#  the last modified file in a given directory and to
#  output the results. It is setup as an infinite loop
#  and in order to cancel execution you need to use
#  ctrl+c or kill the terminal window.
#
#  The loop sleeps for 60 seconds after execution in
#  order to give the analyser enough time and also
#  for 2-3 spills to pass so that there actually is
#  something worth analysing.
# 
#  To run the script use the command
#  ./RunTB <InputFileDirectory> <OutputDirectory>
#
#######################################################


while true
do
    if [[ -z $1 ]]
    then
	echo 'Input file directory not specified.'
	echo 'Please specify input file directory as 1st command line argument and output directory as 2nd.'
	exit 0
    fi

    if [[ -z $2 ]]
    then
	echo 'Output directory not specified.'
	echo 'Please specify input file directory as 1st command line argument and output directory as 2nd.'
	exit 0
    fi

    if [[ -z $3 ]]
    then
	echo 'Starting analysis...'
    else
	echo 'Too many arguments.'
	echo 'Please specify input file directory as 1st command line argument and output directory as 2nd.'
	echo 'Check quotation marks if there are spaces present in your paths or filenames.'
	exit 0
    fi


    InDir="$1"
    OutDir="$2"

    filename=$(ls $InDir -t | head -n1)

    if [ ! -d $OutDir/$filename/ ]
    then
	mkdir -p $OutDir/$filename/
    fi

    # if [ ! -d $OutDir/$filename/ ]
    # then
    #     mkdir -p $OutDir/Plots/$filename/
    # fi
    
    ./ReadVMMData_exe $InDir/$filename
    mv output.root $OutDir/$filename/$filename.root
    mv TotalCharge.png $OutDir/$filename/TotalCharge.png
    mv ChargeOccupancyMap.png $OutDir/$filename/ChargeOccupancyMap.png

    xdg-open $OutDir/$filename/TotalCharge.png
    xdg-open $OutDir/$filename/ChargeOccupancyMap.png
    
    echo "Sleeping for 60 seconds..."
    sleep 60
    echo ; echo
done
