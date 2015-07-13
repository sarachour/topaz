#!/bin/bash

SUMMARY="summary.txt"

cdir=$PWD
rm $SUMMARY
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
  
  if [ "$KIND" = "ldet" ];
  then 
	RATES=""
	KIND="ldet"
	for ldfolder in  `ls output/$folder/ | grep "data"`
	do
		cd output/$folder/$ldfolder
		if [ ! -f "interf.det.txt" ];
		then 
			echo "detected no interf file... working...."
			tpz_det ldet.out interf 0 > interf.det.txt
		fi
		if [ ! -f "poteng.det.txt" ];
		then 
			echo "detected no poteng file... working...."
			tpz_det ldet.out poteng 1 > poteng.det.txt
		fi
		RATE=$(cat interf.det.txt | grep -E "Percent Errors Undetected:[ 0-9\.]+%$" | grep -o -E "[0-9\.]+")
		RATES=$RATES","$RATE
		cd $cdir
	done
	echo "$PROB,$BS,$KIND$RATES" >> $SUMMARY
  fi
  
  #echo $folder
  #echo $PROB $BS kind=$KIND
  
done
cat summary.txt
proc_batch_job.py
