#!/bin/bash


#gdb --args  src/TrackingBenchmark/bodytrack inputs/sequenceB_1 4 1 4000 5 0 0 1
#src/TrackingBenchmark/bodytrack inputs/sequenceB_1 4 1 4000 5 0 0 1
#!/bin/bash
NFRAMES=1
NPOSES=8000
TOPAZ_ARGS=$2
TAGS=$3
INPUT=$4
SEED=$5
PIN_ARGS="$1 -s $SEED"


source sutil_make_dirs.sh

function createDirectories {
	baseCreateDirectories $1
}

function updateDirectories {
	baseUpdateDirectories $1 $2
	outdir=$1
	suff=$2
	rm -rf output/$outdir/frames.$suff
	mkdir output/$outdir/frames.$suff
	mv *.bmp output/$outdir/frames.$suff/
	mv poses.txt output/$outdir/poses.$suff.txt
	mv err.txt output/$outdir/err.$suff.txt
	mv log.txt output/$outdir/log.$suff.txt
}

OUTDIR=$TAGS
SUFFIX="$INPUT.$SEED"
#sutil_run_script.sh run_body.sh 1 4 none iact-med-static
#sutil_run_script.sh run_body.sh 1 6 none iact-med-dynamic
#sutil_run_script.sh run_body.sh 1 6 reexec:t=scar,b=5,p=0.10 iact-med-dynamic d

createDirectories $OUTDIR
tpzrun $PIN_ARGS -- src/TrackingBenchmark/bodytrack $TOPAZ_ARGS @ inputs/sequenceB_$INPUT 4 $NFRAMES $NPOSES 5 0 0 1 > log.txt
mv inputs/sequenceB_$INPUT/*.bmp .
mv inputs/sequenceB_$INPUT/poses.txt .
./diff_outputs.py output/perfect.none*/poses.$INPUT.*.txt poses.txt > err.txt
updateDirectories $OUTDIR $SUFFIX

