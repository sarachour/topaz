#!/bin/bash

OP=$1
PARAM=$2
MACHINE=$3
FLAGS=$4
IN="input$PARAM"

#MACHINE="sram3.cfg"
echo "USAGE: KIND INPUT MACHINE 'FLAGS[g=--godmode t=--time d=--log-detector]'"
LOGS=""
TAGS="TAGS"
CTRL="c"
if [[ $FLAGS == *g* ]]
then
	LOGS=$LOGS" --godmode"
	TAGS=$TAGS".god"
fi
if [[ $FLAGS == *t* ]]
then
	LOGS=$LOGS" --time"
	TAGS=$TAGS".ltime"
fi
if [[ $FLAGS == *d* ]]
then
	LOGS=$LOGS" --log-detector"
	TAGS=$TAGS".ldet"
fi
if [[ $OP == *replace* ]]
then
	AMOUNT=$(echo "$OP" | grep -o "[0-9\.]*")
	LOGS=$LOGS" --detect "$CTRL" --detect-target "$AMOUNT
	TAGS=$TAGS".ctrl."$AMOUNT
fi
if [[ $OP == *preplace* ]]
then
	AMOUNT=$(echo "$OP" | grep -o "[0-9\.]*")
	LOGS=$LOGS"  --prob-accept"
	TAGS=$TAGS".pa"
fi
if [[ $OP == *discard* ]]
then
	AMOUNT=$(echo "$OP" | grep -o "[0-9\.]*")
	LOGS=$LOGS" --discard --detect "$CTRL" --detect-target "$AMOUNT
	TAGS=$TAGS".ctrl."$AMOUNT
fi

if [[ $OP == *pdiscard* ]]
then
	AMOUNT=$(echo "$OP" | grep -o "[0-9\.]*")
	LOGS=$LOGS" --prob-accept"
	TAGS=$TAGS".pa"
fi
#MACHINE="sram3.cfg"



function createDirectories {
	
	export TOPAZ_ENABLE_LOGGING=0
	if [ ! -d output ]; then
		mkdir output
	fi
	if [ ! -d output/$1 ]; then
		mkdir output/$1 
	fi
	if [ ! -d profile ]; then
		mkdir profile
	fi
	if [ ! -d logs ]; then
		mkdir logs
	fi
}

function cleanupDirectories {
	suff=$2
	if [ ! -d output ]; then
		mkdir output
	fi
	if [ ! -d output/$1 ]; then
		mkdir output/$1 
	fi
	if [ ! -d output/$1/timers.$suff ]; then
		mkdir output/$1/timers.$suff
	fi
	if [ ! -d output/$1/logs.$suff ]; then
		mkdir output/$1/logs.$suff
	fi
	if [ ! -d output/$1/frames.$suff ]; then
		mkdir output/$1/frames.$suff
	fi
	if [ -d profile ]; then
		rm -rf output/$1/profile
		mv profile output/$1/profile.$suff
	fi
	if [ -d logs ]; then
		rm -rf output/$1/logs
		mv logs output/$1/logs.$suff
	fi
	mv timer.*.out output/$1/timers.$suff/
	mv task.*.out output/$1/logs.$suff/
	mv ldet.out output/$1/logs.$suff/
	mv frame*.png output/$1/frames.$suff/
}

echo "MACHINE : $MACHINE"


if [[ "$OP" == *analyze* ]]
then
	MAINTAG=$(echo "$OP" | grep -o ":[A-Za-z]*" | sed "s/://g" | sed "s/^p//g")
	
	if [[ $MAINTAG == *correct* ]]
	then
		TAGS=$TAGS
	else
		TAGS=$TAGS.$MACHINE
	fi
	echo "FLAGS : $LOGS"
	echo "MAIN TAG: " $MAINTAG
	
	echo "What kind of statistic would you like to calculate [e=energy, d=distributions, s=stats]"
	read action
	
	distdir=output/$MAINTAG.$TAGS/logs.$PARAM
	endir=output/$MAINTAG.$TAGS/timers.$PARAM
	if [[ $action == *e* ]]
	then
		tpz_enrgy $endir > $endir/energy_summary.txt
	fi
	if [[ $action == *d* ]]
	then
		tpz_dist $distdir/ldet.out 0 0 0
		tpz_dist $distdir/ldet.out 0 0 1
		tpz_dist $distdir/ldet.out 0 0 2
		tpz_dist $distdir/ldet.out 0 1 0
		tpz_dist $distdir/ldet.out 0 1 1
		tpz_dist $distdir/ldet.out 0 1 2
		tpz_dist $distdir/ldet.out 0 2 0
		tpz_dist $distdir/ldet.out 0 3 0
	fi
	if [[ $action == *s* ]]
	then
		tpz_stat $distdir 0 > $distdir/task_summary.0.txt
	fi
	exit 0
else
	echo "FLAGS : $LOGS"
	read -p "press enter to continue"
fi

if [ "$OP" == "debug" ]
then
	outdir="debug".$TAGS
	createDirectories $outdir
	cat inputs/$IN | tpzrun -d -- ./src/t.barnes $LOGS @ > output/$outdir/log.$PARAM.txt && mv pos.txt output/$outdir/pos.$PARAM.txt
	tpzbarnes.py output/$outdir/graph.$PARAM.txt output/$outdir/pos.$PARAM.txt
	cleanupDirectories $outdir $PARAM
fi

if [ "$OP" == "correct" ]
then
	outdir="correct".$TAGS
	createDirectories $outdir
	echo "cat inputs/$IN | tpzrun -e -f perfect.cfg -- ./t.barnes $LOGS @ > output/$outdir/log.$PARAM.txt && mv pos.txt output/$outdir/pos.$PARAM.txt"
	cat inputs/$IN | tpzrun -e -f perfect.cfg -- ./src/t.barnes $LOGS @ > output/$outdir/log.$PARAM.txt && mv pos.txt output/$outdir/pos.$PARAM.txt
	tpzbarnes.py output/$outdir/graph.$PARAM.txt output/$outdir/pos.$PARAM.txt
	tpzplot output/$outdir/graph.$PARAM.txt;
	cleanupDirectories $outdir $PARAM
fi


if [ "$OP" == "basic" ]
then
	outdir="basic".$TAGS.$MACHINE
	corrdir="correct.TAGS"
	createDirectories $outdir
	cat inputs/$IN | tpzrun -e -f $MACHINE  -- ./src/t.barnes $LOGS @ > output/$outdir/log.$PARAM.txt && mv pos.txt output/$outdir/pos.$PARAM.txt
	tpzbarnes.py output/$outdir/graph.$PARAM.txt output/$corrdir/pos.$PARAM.txt output/$outdir/pos.$PARAM.txt
	./diff_output.py output/$corrdir/pos.$PARAM.txt output/$outdir/pos.$PARAM.txt > output/$outdir/err.$PARAM.txt
	tpzplot output/$outdir/graph.$PARAM.txt;
	cleanupDirectories $outdir $PARAM
fi

if [[ "$OP" == *replace* ]]
then
	outdir="replace".$TAGS.$MACHINE
	corrdir="correct.TAGS"
	createDirectories $outdir
	echo "./t.barnes $LOGS @ > output/$outdir/log.$PARAM.txt && mv pos.txt output/$outdir/pos.$PARAM.txt"
	cat inputs/$IN | tpzrun -e -f $MACHINE  -- ./src/t.barnes $LOGS @ > output/$outdir/log.$PARAM.txt && mv pos.txt output/$outdir/pos.$PARAM.txt
	tpzbarnes.py output/$outdir/graph.$PARAM.txt output/$corrdir/pos.$PARAM.txt output/$outdir/pos.$PARAM.txt
	./diff_output.py output/$corrdir/pos.$PARAM.txt output/$outdir/pos.$PARAM.txt > output/$outdir/err.$PARAM.txt
	tpzplot output/$outdir/graph.$PARAM.txt;
	cleanupDirectories $outdir  $PARAM
fi

if [[ "$OP" == *discard* ]]
then
	outdir="discard".$TAGS.$MACHINE
	createDirectories $outdir
	cat inputs/$IN | tpzrun -e -f $MACHINE  -- ./src/t.barnes $LOGS --discard @ > output/$outdir/log.$PARAM.txt && mv pos.txt output/$outdir/pos.$PARAM.txt
	tpzbarnes.py output/$outdir/graph.$PARAM.txt output/$outdir/pos.$PARAM.txt
	cleanupDirectories $outdir $PARAM
fi

