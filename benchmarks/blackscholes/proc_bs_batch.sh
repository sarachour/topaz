#!/bin/bash

SUMMARY="summary.txt"

cdir=$PWD
echo "target-prob,block-size,kind,seed1,seed2,seed3" > $SUMMARY
OUTPUT=output-batch64

for folder in `ls $OUTPUT | grep iact`
do
  PROB=$(echo $folder | grep -o -E "p[0-9]+(\.[0-9]*)?" | sed s/p//g)
  BS=$(echo $folder | grep -o -E "b[0-9]+" | sed s/b//g)
  KIND=$(echo $folder | grep -o -E "[a-z]+$")
  
  if [ "$KIND" = "ltime" ];
  then 
	ERRORS=""
	for efile in  `ls $OUTPUT/$folder/err*`
	do
		ERROR=$(cat $efile | grep -E "Percent Price:[ 0-9\.\-e]+$" | grep -o -E "[0-9\.\-e]+$")
		#ERROR=$(cat $efile | grep -E "Number Errors:[ 0-9\.]+$" | grep -o -E "[0-9\.]+$")
		echo "$efile : $ERROR"
		cat $efile
		ERRORS=$ERRORS","$ERROR
	done
	echo "$PROB,$BS,normal$ERRORS" >> $SUMMARY
  fi
  
  if [ "$KIND" = "ldet" ];
  then 
	RATES=""
	KIND="ldet"
	ERRS=""
	for ldfolder in  `ls $OUTPUT/$folder/ | grep "data"`
	do
		cd $OUTPUT/$folder/$ldfolder
		if [ ! -f "det.txt" ];
		then 
			echo "detected no detector file... working...."
			tpz_det ldet.out graph 0 > det.txt
		fi
		if [ ! -f "stat.txt" ];
		then 
			echo "detected no stat file... working...."
			tpz_det_stats ldet.out 0 > stat.txt
		fi
		RATE=$(cat det.txt | grep -E "Percent Errors Detected:[ 0-9\.]+%$" | grep -o -E "[0-9\.]+")
		ERR=$(cat stat.txt | grep -E "^TOTAL[ 0-9\.e\-]+$" | grep -o -E "[0-9\.e\-]+")
		ERRS=$ERRS","$ERR
		RATES=$RATES","$RATE
		cd $cdir
	done
	echo "$PROB,$BS,$KIND$RATES" >> $SUMMARY
	echo "$PROB,$BS,ldeterr$ERRS" >> $SUMMARY
  fi
  
  
  if [ "$KIND" = "ltime" ];
  then 
	RATES=""
	TOPRATES=""
	KIND="ltime"
	for ldfolder in  `ls $OUTPUT/$folder/ | grep "timers"`
	do
		cd $OUTPUT/$folder/$ldfolder
		if [ ! -f "energy.txt" ];
		then 
			hwdir=$(echo $ldfolder | sed "s/timers/profile/g")
			echo "no energy file... working...."
			tpz_energy ../$hwdir . > energy.txt
		fi
		RATE=$(cat energy.txt | grep -E "^With Outdet Savings" | grep -v "Topaz" | grep -o -E "[0-9\.\-]+")
		RATES=$RATES","$RATE
		RATE=$(cat energy.txt | grep -E "^With Outdet Savings" | grep "Topaz" | grep -o -E "[0-9\.\-]+")
		TOPRATES=$TOPRATES","$RATE
		cd $cdir
	done
	
	echo "$PROB,$BS,$KIND$RATES" >> $SUMMARY
	echo "$PROB,$BS,ltime-tpz$TOPRATES" >> $SUMMARY
	
	
  fi
  
  
  #echo $folder
  #echo $PROB $BS kind=$KIND
  
done

proc_batch_job.py
