OP=$1
NMOL=$2
NTIME=$3
RANDIDX=$4
MACHINE=$5
FLAGS=$6
IN1="LWI5.$NMOL.$NTIME"
IN2="random.in.$RANDIDX"

echo "USAGE: KIND NMOL NTIME RANDIDX MACHINE 'FLAGS[g=--godmode t=--time d=--log-detector]'"
LOGS=""
TAGS="TAGS"
CTRL="c"
if [[ $FLAGS == *c* ]]
then
	LOGS=$LOGS" --cross-detector"
	TAGS=$TAGS".cross"
fi
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
	LOGS=$LOGS" --prob-accept"
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
	
	PARAM="$NMOL.$NTIME.$RANDIDX"
	distdir=output/$MAINTAG.$TAGS/logs.$PARAM
	endir=output/$MAINTAG.$TAGS/timers.$PARAM
	#energy calculation
	if [[ $action == *e* ]]
	then
		tpz_enrgy $endir > $endir/energy_summary.txt
	fi
	
	if [[ $action == *s* ]]
	then
		tpz_stat $distdir 0 > $distdir/task_summary.0.txt
		tpz_stat $distdir 1 > $distdir/task_summary.1.txt
	fi
	
	if [[ $action == *d* ]]
	then
		for i in `seq 0 8`
		do
			tpz_dist $distdir/ldet.out 0 0 $i
		done
		for i in `seq 0 8`
		do
			tpz_dist $distdir/ldet.out 0 1 $i
		done
		tpz_dist $distdir/ldet.out 0 2 0
		tpz_dist $distdir/ldet.out 0 3 0
		
		for i in `seq 0 2`
		do
			tpz_dist $distdir/ldet.out 1 0 $i
		done
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
	tpzrun -d -- ./t.water $LOGS @ inputs/$IN1 inputs/$IN2 output/$outdir/pos.$NMOL.$NTIME.$RANDIDX.txt output/$outdir/en.$NMOL.$NTIME.$RANDIDX.txt > output/$outdir/log.$NMOL.$NTIME.$RANDIDX.txt
	
	cleanupDirectories $outdir "$NMOL.$NTIME.$RANDIDX"
fi

if [ "$OP" == "correct" ]
then
	outdir="correct".$TAGS
	createDirectories $outdir
	tpzrun -e -f perfect.cfg -- ./t.water $LOGS @ inputs/$IN1 inputs/$IN2 output/$outdir/pos.$NMOL.$NTIME.$RANDIDX.txt output/$outdir/en.$NMOL.$NTIME.$RANDIDX.txt > output/$outdir/log.$NMOL.$NTIME.$RANDIDX.txt
	tpzwater.py output/$outdir/graph.$NMOL.$NTIME.$RANDIDX.txt output/$outdir/pos.$NMOL.$NTIME.$RANDIDX.txt
	tpzplot output/$outdir/graph.$NMOL.$NTIME.$RANDIDX.txt
	cleanupDirectories $outdir "$NMOL.$NTIME.$RANDIDX"
fi

if [ "$OP" == "basic" ]
then
	outdir="basic".$TAGS.$MACHINE
	createDirectories $outdir
	tpzrun -e -f $MACHINE -- ./t.water $LOGS @ inputs/$IN1 inputs/$IN2 output/$outdir/pos.$NMOL.$NTIME.$RANDIDX.txt output/$outdir/en.$NMOL.$NTIME.$RANDIDX.txt > output/$outdir/log.$NMOL.$NTIME.$RANDIDX.txt
	./diff_output.py output/correct.TAGS/pos.$NMOL.$NTIME.$RANDIDX.txt output/$outdir/pos.$NMOL.$NTIME.$RANDIDX.txt > output/$outdir/err.$NMOL.$NTIME.$RANDIDX.txt
	tpzwater.py output/$outdir/graph.$NMOL.$NTIME.$RANDIDX.txt output/correct.TAGS/pos.$NMOL.$NTIME.$RANDIDX.txt output/$outdir/pos.$NMOL.$NTIME.$RANDIDX.txt
	tpzplot output/$outdir/graph.$NMOL.$NTIME.$RANDIDX.txt
	#tpzwater.py output/$outdir/graph.$NMOL.$NTIME.$RANDIDX.txt output/$outdir/pos.$NMOL.$NTIME.$RANDIDX.txt
	cleanupDirectories $outdir "$NMOL.$NTIME.$RANDIDX"
fi

if [[ "$OP" == *replace* ]]
then
	outdir="replace".$TAGS.$MACHINE
	createDirectories $outdir
	tpzrun -e -f $MACHINE -- ./t.water $LOGS @ inputs/$IN1 inputs/$IN2 output/$outdir/pos.$NMOL.$NTIME.$RANDIDX.txt output/$outdir/en.$NMOL.$NTIME.$RANDIDX.txt > output/$outdir/log.$NMOL.$NTIME.$RANDIDX.txt
	./diff_output.py output/correct.TAGS/pos.$NMOL.$NTIME.$RANDIDX.txt output/$outdir/pos.$NMOL.$NTIME.$RANDIDX.txt > output/$outdir/err.$NMOL.$NTIME.$RANDIDX.txt
	 tpzwater.py output/$outdir/graph.$NMOL.$NTIME.$RANDIDX.txt output/correct.TAGS/pos.$NMOL.$NTIME.$RANDIDX.txt output/$outdir/pos.$NMOL.$NTIME.$RANDIDX.txt
	tpzplot output/$outdir/graph.$NMOL.$NTIME.$RANDIDX.txt
	cleanupDirectories $outdir "$NMOL.$NTIME.$RANDIDX"
fi

if [[ "$OP" == *discard* ]]
then
	outdir="discard".$TAGS.$MACHINE
	createDirectories $outdir
	tpzrun -e -f $MACHINE -- ./t.water $LOGS @ inputs/$IN1 inputs/$IN2 output/$outdir/pos.$NMOL.$NTIME.$RANDIDX.txt output/$outdir/en.$NMOL.$NTIME.$RANDIDX.txt > output/$outdir/log.$NMOL.$NTIME.$RANDIDX.txt
	#tpzwater.py output/$outdir/graph.$NMOL.$NTIME.$RANDIDX.txt output/$outdir/pos.$NMOL.$NTIME.$RANDIDX.txt
	cleanupDirectories $outdir "$NMOL.$NTIME.$RANDIDX"
fi

