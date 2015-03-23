#!/bin/bash


#gdb --args  src/TrackingBenchmark/bodytrack inputs/sequenceB_1 4 1 4000 5 0 0 1
#src/TrackingBenchmark/bodytrack inputs/sequenceB_1 4 1 4000 5 0 0 1
#!/bin/bash
NFRAMES=2
PIN_ARGS=$1
TOPAZ_ARGS=$2
TAGS=$3
INPUT=$4
SEED=$5

source sutil_make_dirs.sh

function createDirectories {
	baseCreateDirectories $1
}

function updateDirectories {
	baseUpdateDirectories $1 $2
	outdir=$1
	suff=$2
	mkdir output/$outdir/frames.$suff
	mv *.bmp output/$outdir/frames.$suff/
	mv poses.txt output/$outdir/poses.$suff.txt
	mv err.txt output/$outdir/err.$suff.txt
	mv log.txt output/$outdir/log.$suff.txt
}

OUTDIR=$TAGS
SUFFIX="$INPUT.$SEED"


#sutil_run_script.sh run_bs.sh 16K 1 reexec:t=scar,b=5,p=0.10 heavy-dram d

createDirectories $OUTDIR
tpzrun $PIN_ARGS -- src/TrackingBenchmark/bodytrack $TOPAZ_ARGS @ inputs/sequence$INPUT 4 $NFRAMES 4000 5 0 0 1 > log.txt
mv inputs/sequence$SEQID/*.bmp .
mv inputs/sequence$SEQID/poses.txt .
./diff_outputs.py output/perfect.none*/poses.$SEQID.*.txt poses.txt > err.txt
updateDirectories $OUTDIR $SUFFIX

