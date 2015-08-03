#!/bin/bash

PIN_ARGS=$1
TOPAZ_ARGS=$2
TAGS=$3
INPUT=$4
SEED=$5

MINCLUST=2; # 2 for tiny, 3 for small, 5 for med, 7 for large
MAXCLUST=4; # 5 for all but med:10, large 12
CHUNKSIZE=500;
CLUSTERSIZE=1000;



source sutil_make_dirs.sh

function createDirectories {
	baseCreateDirectories $1
}

function updateDirectories {
	baseUpdateDirectories $1 $2
	outdir=$1
	suff=$2
	mv diagram.png output/$outdir/diagram.$suff.png
	mv clust.txt output/$outdir/clust.$suff.txt
	mv err.txt output/$outdir/err.$suff.txt
	mv log.txt output/$outdir/log.$suff.txt
}

OUTDIR=$TAGS
SUFFIX="$INPUT.$SEED"

#sutil_run_script.sh run_stream.sh large 1 reexec:t=scar,b=5,p=0.10 heavy-dram d

createDirectories $OUTDIR
tpzrun $PIN_ARGS  -- src/streamcluster $TOPAZ_ARGS @ $MINCLUST $MAXCLUST $CHUNKSIZE $CLUSTERSIZE inputs/$INPUT.txt clust.txt 1 &> log.txt
./visualize.py inputs/$INPUT.txt clust.txt
./eval_output.py inputs/$INPUT.txt clust.txt > err.txt
updateDirectories $OUTDIR $SUFFIX

