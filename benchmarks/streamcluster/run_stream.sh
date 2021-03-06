#!/bin/bash

TOPAZ_ARGS=$2
TAGS=$3
INPUT=$4
SEED=$5
PIN_ARGS="$1 -s $SEED"


MINCLUST=3; # 2 for tiny, 3 for small, 5 for med, 7 for large
MAXCLUST=7; # 5 for all but med:10, large 12
CHUNKSIZE=400
CLUSTERSIZE=500;


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
echo "-----------"
echo "tpzrun $PIN_ARGS  -- src/streamcluster $TOPAZ_ARGS @ $MINCLUST $MAXCLUST 2 0 $CHUNKSIZE $CLUSTERSIZE inputs/$INPUT.txt clust.txt &> log.txt"
echo "-----------"
tpzrun $PIN_ARGS  -- src/streamcluster $TOPAZ_ARGS @ $MINCLUST $MAXCLUST 2 0 $CHUNKSIZE $CLUSTERSIZE inputs/$INPUT.txt clust.txt &> log.txt
./visualize.py inputs/$INPUT.txt clust.txt
./eval_output.py inputs/$INPUT.txt clust.txt > err.txt
updateDirectories $OUTDIR $SUFFIX

