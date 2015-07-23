#!/bin/bash
FLAG=$1
CAT=$2
OUTPUT=output-$FLAG

echo "benchmark,aov,target,OE,TE,M,D,OA,TA"
for dir in `ls $OUTPUT/*/timers* | grep ltime | sed "s/:$//g"`;
do
	REEXEC_RATE=$(echo $dir | grep -E -o "p[0-9\.]+" | grep -E -o "[0-9\.]+[0-9]+")
	file=$dir/energy.txt
	OE=$(cat $file | grep -o -E "OE= [0-9\.]+"| grep -o -E "[0-9\.]+[0-9]+")
	OA=$(cat $file | grep -o -E "OA= [0-9\.]+"| grep -o -E "[0-9\.]+[0-9]+")
	TE=$(cat $file | grep -o -E "TE= [0-9\.]+"| grep -o -E "[0-9\.]+[0-9]+")
	TA=$(cat $file | grep -o -E "TA= [0-9\.]+"| grep -o -E "[0-9\.]+[0-9]+")
	M=$(cat $file | grep -o -E "M= [0-9\.]+"| grep -o -E "[0-9\.]+[0-9]+")
	D=$(cat $file | grep -o -E "D= [0-9\.]+"| grep -o -E "[0-9\.]+[0-9]+")
	echo $CAT,$FLAG,$REEXEC_RATE,$OE,$TE,$M,$D,$OA,$TA
done

