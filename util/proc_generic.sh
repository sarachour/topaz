#!/bin/bash

SUMMARY="summary.txt"
OUTPUT=$1
CMD=$2

cdir=$PWD
echo "target-prob,block-size,kind,seed1,seed2,seed3" > $SUMMARY

for folder in `ls $OUTPUT | grep iact`
do
  PROB=$(echo $folder | grep -o -E "p[0-9]+(\.[0-9]*)?" | sed s/p//g)
  BS=$(echo $folder | grep -o -E "b[0-9]+" | sed s/b//g)
  KIND=$(echo $folder | grep -o -E "[a-z]+$")
  MACH=$(echo $folder | grep -o -E "iact-[A-Za-z0-9\-]+" | sed "s/iact-//g")
  
  if [ "$KIND" = "ltime" ];
  then 
	ERRORS=""
	for efile in  `ls $OUTPUT/$folder/err*`
	do
		ERROR=$(eval "cat $efile | $CMD")
		ERRORS=$ERRORS","$ERROR
	done
	echo "$MACH,$PROB,$BS,normal$ERRORS" >> $SUMMARY
  fi
  
  if [ "$KIND" = "ldet" ];
  then 
	RATES=""
	RATES1=""
	RATES2=""
	KIND="ldet"
	ERRS=""
	ERRS2=""
	for ldfolder in  `ls $OUTPUT/$folder/ | grep "data"`
	do
		cd $OUTPUT/$folder/$ldfolder
		if [ ! -f "det.txt" ];
		then 
			echo "detected no detector file... working...."
			tpz_det ldet.out taskset0 0 > det.txt
			tpz_det ldet.out taskset1 1 > det2.txt
		fi
		if [ ! -f "stat.txt" ];
		then 
			echo "detected no stat file... working...."
			tpz_det_stat ldet.out 0 > stat.txt
			tpz_det_stat ldet.out 1 > stat2.txt
		fi
		X=$(cat det.txt | grep -E "abs,errors-caught," | grep -o -E "[0-9\.]+")
		RATES=$RATES","$X
		X=$(cat det.txt | grep -E "abs,rejs-errors," | grep -o -E "[0-9\.]+")
		RATES1=$RATES1","$X
		X=$(cat det.txt | grep -E "abs,rej-rate," | grep -o -E "[0-9\.]+")
		RATES2=$RATES2","$X
		X=$(cat stat.txt | grep -E "abs,err" | grep -o -E "[0-9\.e\-]+$")
		ERRS=$ERRS","$X
		X=$(cat stat.txt | grep -E "act,err" | grep -o -E "[0-9\.e\-]+$")
		ERRS2=$ERRS2","$X
		cd $cdir
	done
	echo "$MACH,$PROB,$BS,ldet-errs$RATES" >> $SUMMARY
	echo "$MACH,$PROB,$BS,ldet-tp$RATES1" >> $SUMMARY
	echo "$MACH,$PROB,$BS,ldet-rate$RATES2" >> $SUMMARY
	echo "$MACH,$PROB,$BS,ldet-aov-err$ERRS" >> $SUMMARY
	echo "$MACH,$PROB,$BS,ldet-task-err$ERRS2" >> $SUMMARY
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
	
	echo "$MACH,$PROB,$BS,$KIND$RATES" >> $SUMMARY
	echo "$MACH,$PROB,$BS,ltime-tpz$TOPRATES" >> $SUMMARY
	
	
  fi
  
  
done
