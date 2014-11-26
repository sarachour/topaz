#!/bin/bash


#gdb --args  src/TrackingBenchmark/bodytrack inputs/sequenceB_1 4 1 4000 5 0 0 1
#src/TrackingBenchmark/bodytrack inputs/sequenceB_1 4 1 4000 5 0 0 1
#!/bin/bash

NFRAMES=2
OP=$1
SEQID=$2
FLAGS=$4
#MACHINE="sram3.cfg"
MACHINE=$3

echo "USAGE: KIND INPUT 'FLAGS[g=--godmode t=--time d=--log-detector]'"
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
if [[ $FLAGS == *p* ]]
then
	CTRL='d'
	TAGS=$TAGS".pdist"
fi
if [[ $OP == *replace* ]]
then
	AMOUNT=$(echo "$OP" | grep -o "[0-9\.]*")
	LOGS=$LOGS" --detect "$CTRL" --detect-target "$AMOUNT
	TAGS=$TAGS".ctrl."$AMOUNT
fi
if [[ $OP == *discard* ]]
then
	AMOUNT=$(echo "$OP" | grep -o "[0-9\.]*")
	LOGS=$LOGS" --discard --detect "$CTRL" --detect-target "$AMOUNT
	TAGS=$TAGS".ctrl."$AMOUNT
fi


function createDirectories {
	
	export TOPAZ_ENABLE_LOGGING=0
	if [ ! -d output ]; then
		mkdir output
	fi
	if [ ! -d output/$1 ]; then
		mkdir output/$1 
	fi
	if [ ! -d output/$1/profile ]; then
		mkdir output/$1/profile
	fi
	if [ ! -d output/$1/logs ]; then
		mkdir output/$1/logs 
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
	echo $1 $2 $3
	if [ ! -d output ]; then
		mkdir output
	fi
	if [ ! -d output/$1 ]; then
		mkdir output/$1 
	fi
	if [ ! -d output/$1/timers.$suff ]; then
		mkdir output/$1/timers.$suff
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
	mv *.bmp output/$1/frames.$suff/
	mv poses.txt output/$1/poses.$suff.txt
}

echo "OPERATION: $OP"
echo "MACHINE : $MACHINE"
PARAM="$SEQID"

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
	
	PARAM="$SEQID"
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
		tpz_dist $distdir/ldet.out 0 1 0
		tpz_dist $distdir/ldet.out 0 2 0
	fi
	if [[ $action == *s* ]]
	then
		tpz_stat $distdir 0 > $distdir/task_summary.0.txt
	fi
	exit 0
else
	echo "FLAGS : $LOGS"
	echo "TAGS : $TAGS"
	read -p "press enter to continue"
fi

if [ "$OP" == "diff" ]
then
    outdir=$3
	compare -metric PSNR output/correct/$IN.ppm output/$outdir/$IN.$MACHINE.ppm output/$outdir/diff.ppm
fi
if [ "$OP" == "diff-det" ]
then
    outdir=$4
    CFG=$3
	compare -metric PSNR output/correct/$IN.ppm output/$outdir/$CFG.$IN.$MACHINE.ppm output/$outdir/diff.ppm
fi

if [ "$OP" == "debug" ]
then
	outdir="debug".$TAGS
	createDirectories $outdir
	tpzrun -d -- src/TrackingBenchmark/bodytrack $LOGS @ inputs/sequence$SEQID 4 $NFRAMES 4000 5 0 0 1
	mv inputs/sequence$SEQID/*.bmp .
	cleanupDirectories $outdir $PARAM
fi

#generate the correct output
if [ "$OP" == "correct" ]
then
	outdir="correct".$TAGS
	createDirectories $outdir
	tpzrun -e -f perfect.cfg -- src/TrackingBenchmark/bodytrack $LOGS @ inputs/sequence$SEQID 4 $NFRAMES 4000 5 0 0 1 >  output/$outdir/log.$PARAM.txt
	mv inputs/sequence$SEQID/*.bmp .
	mv inputs/sequence$SEQID/poses.txt .
	cleanupDirectories $outdir $PARAM
fi

#generate basic output.
if [ "$OP" == "basic" ]
then
	outdir="basic".$TAGS.$MACHINE
	corrdir="correct.TAGS"
	createDirectories $outdir
	tpzrun -e -f $MACHINE -- src/TrackingBenchmark/bodytrack $LOGS @ inputs/sequence$SEQID 4 $NFRAMES 4000 5 0 0 1 >  output/$outdir/log.$PARAM.txt
	mv inputs/sequence$SEQID/*.bmp .
	mv inputs/sequence$SEQID/poses.txt .
	./diff_outputs.py output/$corrdir/poses.$SEQID.txt output/$outdir/poses.$SEQID.txt > output/$outdir/err.$SEQID.txt
	cleanupDirectories $outdir $PARAM
fi

if [[ "$OP" == *replace* ]]
then
	outdir="replace".$TAGS.$MACHINE
	corrdir="correct.TAGS"
	createDirectories $outdir
	tpzrun -e -f $MACHINE -- src/TrackingBenchmark/bodytrack $LOGS @ inputs/sequence$SEQID 4 $NFRAMES 4000 5 0 0 1 >  output/$outdir/log.$PARAM.txt
	mv inputs/sequence$SEQID/*.bmp .
	mv inputs/sequence$SEQID/poses.txt .
	./diff_outputs.py output/$corrdir/poses.$SEQID.txt output/$outdir/poses.$SEQID.txt > output/$outdir/err.$SEQID.txt
	cleanupDirectories $outdir $PARAM
fi

if [[ "$OP" == *discard* ]]
then
	outdir="discard".$TAGS.$MACHINE
	createDirectories $outdir
	#tpzrun -e -f $MACHINE  -- ./t.imgops --detect @ 2.0 inputs/baboon.ppm out.ppm > output/$outdir/log.$MACHINE.txt && mv out.ppm output/$outdir/out.ppm
	tpzrun -e -f $MACHINE -- ./t.imgops $LOGS  @ 2.0 inputs/$IN.ppm out.ppm > output/$outdir/log.$IN.txt && mv out.ppm output/$outdir/$IN.ppm
	mv inputs/sequence$SEQID/*.bmp .
	mv inputs/sequence$SEQID/poses.txt .
	cleanupDirectories $outdir $IN
fi
