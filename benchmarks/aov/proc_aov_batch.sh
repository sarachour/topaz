#!/bin/bash

SUMMARY="summary.txt"

cdir=$PWD
echo "category,kind,target-prob,block-size,kind,seed1,seed2,seed3" > $SUMMARY
for category in `ls output`
do 
	for kind in `ls output/$category`
	do
		output="output/$category/$kind"
		for folder in `ls output/$category/$kind | grep iact`
		do
		  PROB=$(echo $folder | grep -o -E "p[0-9]+(\.[0-9]*)?" | sed s/p//g)
		  BS=$(echo $folder | grep -o -E "b[0-9]+" | sed s/b//g)
		  KIND=$(echo $folder | grep -o -E "[a-z]+$")
		  
		  if [ "$KIND" = "ltime" ];
		  then 
			ERRORS=""
			for efile in  `ls $output/$folder/err*`
			do
				ERROR=$(cat $efile | grep -E "Percent Price:[ 0-9\.]+$" | grep -o -E "[e0-9\.\-]+$")
				#ERROR=$(cat $efile | grep -E "Number Errors:[ 0-9\.]+$" | grep -o -E "[0-9\.]+$")
				ERRORS=$ERRORS","$ERROR
			done
			echo "$category,$kind,$PROB,$BS,normal$ERRORS" >> $SUMMARY
		  fi
		  
		  if [ "$KIND" = "ldet" ];
		  then 
			RATES=""
			KIND="ldet"
			for ldfolder in  `ls $output/$folder/ | grep "data"`
			do
				cd $output/$folder/$ldfolder
				if [ ! -f "det.txt" ];
				then 
					echo "detected no detector file... working...."
					tpz_det ldet.out graph 0 > det.txt
				fi
				RATE=$(cat det.txt | grep -E "Percent Errors Detected:[ 0-9\.]+%$" | grep -o -E "[0-9\.]+")
				RATES=$RATES","$RATE
				cd $cdir
			done
			echo "$category,$kind,$PROB,$BS,$KIND$RATES" >> $SUMMARY
		  fi
		  
		  if [ "$KIND" = "ltime" ];
		  then 
			RATES=""
			KIND="ltime"
			TRATES=""
			for ldfolder in  `ls $output/$folder/ | grep "timers"`
			do
				cd $output/$folder/$ldfolder
				if [ ! -f "energy.txt" ];
				then 
					echo "detected no energy file... working...."
					hwdir=$(echo $ldfolder | sed s/timers/profile/g)
					tpz_energy ../$hwdir . > energy.txt
				fi
				RATE=$(cat energy.txt | grep -E "^With Outdet Savings" | grep -v "Topaz" | grep -o -E "[0-9\.\-]+")
				TRATE=$(cat energy.txt | grep -E "^With Outdet Savings" | grep "Topaz" | grep -o -E "[0-9\.\-]+")
				RATES=$RATES","$RATE
				TRATES=$TRATES","$TRATE
				cd $cdir
			done
			echo "$category,$kind,$PROB,$BS,$KIND$RATES" >> $SUMMARY
			echo "$category,$kind,$PROB,$BS,ltime-tpz$TRATES" >> $SUMMARY
		  fi
		  
		done
	done
done

./proc_aov_batch.py
