OP=$1
FLAGS=$4
#MACHINE="sram3.cfg"
MACHINE=$3
SIZE=$2
IN="in_$SIZE"

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

if [[ $FLAGS == *c* ]]
then
	LOGS=$LOGS" --scar-detector"
	TAGS=$TAGS".scar"
fi

if [[ $FLAGS == *d* ]]
then
	LOGS=$LOGS" --log-detector"
	TAGS=$TAGS".ldet"
fi
if [[ $FLAGS == *p* ]]
then
	CTRL='d'
	TAGS=$TAGS".pdist"
fi
if [[ $OP == *replace* ]]
then
	AMOUNT=$(echo "$OP" | grep -o "p[0-9\.]*" | grep -o "[0-9\.]*")
	BLKS=$(echo "$OP" | grep -o "b[0-9]*" | grep -o "[0-9\.]*")
	LOGS=$LOGS" --detect "$CTRL" --detect-target "$AMOUNT" --detect-blocks "$BLKS
	TAGS=$TAGS".ctrl.p"$AMOUNT".b"$BLKS
fi

if [[ $OP == *discard* ]]
then
	AMOUNT=$(echo "$OP" | grep -o "p[0-9\.]*" | grep -o "[0-9\.]*")
	BLKS=$(echo "$OP" | grep -o "b[0-9]*" | grep -o "[0-9\.]*")
	LOGS=$LOGS" --detect "$CTRL" --detect-target "$AMOUNT" --detect-blocks "$BLKS
	TAGS=$TAGS".ctrl.p"$AMOUNT".b"$BLKS
fi


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
		rm -rf output/$1/timers.$suff
		mkdir output/$1/timers.$suff
	fi
	if [ ! -d output/$1/logs.$suff ]; then
		mkdir output/$1/logs.$suff
	fi
	if [ -d profile ]; then
		rm -rf output/$1/profile.$suff
		mv profile output/$1/profile.$suff
	fi
	if [ -d logs ]; then
		rm -rf output/$1/logs.$suff
		mv logs output/$1/logs.$suff
	fi
	mv timer.*.out output/$1/timers.$suff/
	mv task.*.out output/$1/logs.$suff/
	mv ldet.out output/$1/logs.$suff/
}

#MACHINE="sram3.cfg"
echo "OPERATION: $OP"
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
	
	PARAM="$SIZE"
	distdir=output/$MAINTAG.$TAGS/logs.$PARAM
	endir=output/$MAINTAG.$TAGS/timers.$PARAM
	#energy calculation
	if [[ $action == *e* ]]
	then
		tpz_enrgy $endir > $endir/energy_summary.txt
	fi
	if [[ $action == *d* ]]
	then
		tpz_dist $distdir/ldet.out 0 0 0
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

if [ "$OP" == "correct" ]
then
	outdir="correct".$TAGS
	createDirectories $outdir
	tpzrun -e -f perfect.cfg -- ./t.blackscholes $LOGS @ 1 inputs/$IN.txt output/$outdir/price.$SIZE.txt  > output/$outdir/log.$SIZE.txt
	cleanupDirectories $outdir $SIZE
fi

if [ "$OP" == "basic" ]
then
	outdir="basic".$TAGS.$MACHINE
	createDirectories $outdir
	tpzrun -e -f $MACHINE  -- ./t.blackscholes $LOGS @ 1 inputs/$IN.txt output/$outdir/price.$SIZE.txt  > output/$outdir/log.$SIZE.txt
	./diff_output.py output/correct.TAGS/price.$SIZE.txt output/$outdir/price.$SIZE.txt > output/$outdir/err.$SIZE.txt
	cleanupDirectories $outdir $SIZE
fi

if [[ "$OP" == *replace* ]]
then
	outdir="replace".$TAGS.$MACHINE
	createDirectories $outdir
	echo "./t.blackscholes $LOGS @ 1 inputs/$IN.txt output/$outdir/price.$CFG.$SIZE.$MACHINE.txt  > output/$outdir/log.$CFG.$SIZE.$MACHINE.txt"
	tpzrun -e -f $MACHINE  -- ./t.blackscholes $LOGS @ 1 inputs/$IN.txt output/$outdir/price.$SIZE.txt  > output/$outdir/log.$SIZE.txt
	./diff_output.py output/correct.TAGS/price.$SIZE.txt output/$outdir/price.$SIZE.txt > output/$outdir/err.$SIZE.txt
	cleanupDirectories $outdir $SIZE
fi

if  [[ "$OP" == *discard* ]]
then
	outdir="discard".$TAGS.$MACHINE
	createDirectories $outdir
	tpzrun -e -f $MACHINE  -- ./t.blackscholes $LOGS @ 1 inputs/$IN.txt output/$outdir/price.$SIZE.txt  > output/$outdir/log.$SIZE.txt
	cleanupDirectories $outdir $SIZE
fi
