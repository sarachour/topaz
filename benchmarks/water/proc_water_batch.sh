#!/bin/bash

SUMMARY="summary.txt"

echo "target-prob,block-size,kind,seed1,seed2,seed3" > $SUMMARY
for folder in `ls output | grep iact`
do
  PROB=$(echo $folder | grep -o -E "p[0-9]+(\.[0-9]*)?" | sed s/p//g)
  BS=$(echo $folder | grep -o -E "b[0-9]+" | sed s/b//g)
  KIND=$(echo $folder | grep -o -E "[a-z]+$")
  
  if [ "$KIND" = "" ];
  then 
	ERRORS=""
	KIND="normal"
	for efile in  `ls output/$folder/err*`
	do
		ERROR=$(cat $efile | grep -E "Average Vector Pos Pct Err:[ 0-9\.]+$" | grep -o -E "[0-9\.]+$")
		ERRORS=$ERRORS","$ERROR
	done
	echo "$PROB,$BS,$KIND$ERRORS" >> $SUMMARY
  fi
  
  
  #echo $folder
  #echo $PROB $BS kind=$KIND
  
done

./proc_water_batch.py
