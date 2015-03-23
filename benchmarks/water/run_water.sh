#!/bin/bash

PIN_ARGS=$1
TOPAZ_ARGS=$2
TAGS=$3
INPUT=$4
SEED=$5


NMOL=$(echo "$INPUT" | grep -o "m[0-9]*" | sed "s/[a-z]//g")
NTIME=$(echo "$INPUT" | grep -o "t[0-9]*" | sed "s/[a-z]//g")
RANDIDX=$(echo "$INPUT" | grep -o "r[0-9]*" | sed "s/[a-z]//g")

IN1="LWI5.$NMOL.$NTIME"
IN2="random.in.$RANDIDX"

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
	mv en.txt output/$outdir/en.$suff.txt
	mv log.txt output/$outdir/log.$suff.txt
	mv graph.txt output/$outdir/graph.$suff.txt
}

OUTDIR=$TAGS
SUFFIX="$INPUT.$SEED"

#sutil_run_script.sh run_water.sh m100t12r2 1 reexec:t=scar,b=5,p=0.10 iact-med-static d

createDirectories $OUTDIR
tpzrun $PIN_ARGS  -- ./t.water $TOPAZ_ARGS @ inputs/$IN1 inputs/$IN2 pos.txt en.txt > log.txt
tpzwater.py graph.txt output/perfect.none*/pos.$INPUT.*.txt pos.txt
./diff_output.py output/perfect.none*/pos.$INPUT.*.txt pos.txt > err.txt
tpzplot graph.txt
updateDirectories $OUTDIR $SUFFIX

