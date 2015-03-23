#!/bin/bash

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
	mv frame* output/$outdir/frames.$suff
	mv pos.txt output/$outdir/pos.$suff.txt
	mv err.txt output/$outdir/err.$suff.txt
	mv log.txt output/$outdir/log.$suff.txt
	mv graph.txt output/$outdir/graph.$suff.txt
}

OUTDIR=$TAGS
SUFFIX="$INPUT.$SEED"

#sutil_run_script.sh run_bs.sh 16K 1 reexec:t=scar,b=5,p=0.10 heavy-dram d

createDirectories $OUTDIR
cat inputs/"input$INPUT" | tpzrun $PIN_ARGS  -- ./src/t.barnes $TOPAZ_ARGS @ > log.txt
tpzbarnes.py graph.txt output/perfect.none*/pos.$INPUT.*.txt pos.txt
./diff_output.py output/perfect.none*/pos.$INPUT.*.txt pos.txt > err.txt
tpzplot graph.txt
updateDirectories $OUTDIR $SUFFIX

