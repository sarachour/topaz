#!/bin/bash

SUMMARY="summary.txt"

cdir=$PWD
echo "category,kind,target-prob,block-size,kind,seed1,seed2,seed3" > $SUMMARY
OUTPUT=output

rm tmp.txt
touch tmp.txt
for category in `ls $OUTPUT`
do 
	for kind in `ls $OUTPUT/$category`
	do
		rm summary.txt
		proc_generic.sh "$OUTPUT/$category/$kind" 'grep -E "Percent Price:[ 0-9\.\-e]+$" | grep -o -E "[0-9\.\-e]+$"'
		cat summary.txt | tail -n +2 | sed -E "s/^/$category,$kind,/g" >> tmp.txt
		cd $cdir
	done
done
mv tmp.txt summary.txt
./proc_aov_batch.py
