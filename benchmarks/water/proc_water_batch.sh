#!/bin/bash

SUMMARY="summary.txt"

cdir=$PWD
rm $SUMMARY
echo "target-prob,block-size,kind,seed1,seed2,seed3" > $SUMMARY
OUTPUT=output-batch5

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
		ERROR=$(cat $efile | grep -E "Average Vector Pos Pct Err:[ 0-9\.e\-]+$" | grep -o -E "[0-9\.e\-]+$")
		echo "$efile : $ERROR"
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
		rm *.stat.txt
		if [ ! -f "interf.det.txt" ];
		then 
			echo "detected no det file... working...."
			tpz_det ldet.out interf 0 > interf.det.txt
			tpz_det ldet.out poteng 1 > poteng.det.txt
		fi
		if [ ! -f "interf.stat.txt" ];
		then 
			echo "detected no stat file... working...."
			tpz_det_stats ldet.out 0 > interf.stat.txt
			tpz_det_stats ldet.out 1 > poteng.stat.txt
		fi
		cp interf.det.txt det.txt
		cp interf.stat.txt stat.txt
		RATE=$(cat interf.det.txt | grep -E "Percent Errors Detected:[ 0-9\.]+%$" | grep -o -E "[0-9\.]+")
		ERR=$(cat interf.stat.txt | grep -E "^TOTAL[ 0-9\.e\-]+$" | grep -o -E "[0-9\.e\-]+")
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
		#rm energy.txt
		if [ ! -f "energy.txt" ];
		then 
			echo "detected no energy file... working...."
			hwdir=$(echo $ldfolder | sed s/timers/profile/g)
			tpz_energy ../$hwdir . > energy.txt
		fi
		RATE=$(cat energy.txt | grep -E "^With Outdet Savings" | grep -v "Topaz" |grep -o -E "[0-9\.\-]+")
		TRATE=$(cat energy.txt | grep -E "^With Outdet Savings" | grep "Topaz" | grep -o -E "[0-9\.\-]+")
		RATES=$RATES","$RATE
		TOPRATES=$TOPRATES","$TRATE
		cd $cdir
	done
	echo "$PROB,$BS,$KIND$RATES" >> $SUMMARY
	echo "$PROB,$BS,ltime-tpz$TOPRATES" >> $SUMMARY
	
  fi
  #echo $folder
  #echo $PROB $BS kind=$KIND
  
done
cat summary.txt
proc_batch_job.py
