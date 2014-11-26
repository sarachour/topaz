#!/bin/bash

BASE=../../..
TOOLS=${BASE}/tools
source "${BASE}/run.base.h.sh"

function run_seq #(numpoints, repetitions)#
{
    NP=$1
    REP=$2
    rm -f SEQ-TIME.${NP}

    for i in `seq 1 ${REP}`; do

	${TOOLS}/time-my.sh -o SEQ-TIME.${NP} -a ./water-seq ../inputs/LWI5.$NP random.in > SEQ-TMP-OUT.${i};
	local stdout=`cat SEQ-TMP-OUT.${i}`
	rm -f SEQ-TMP-OUT-${i}
	
	export PYTHONPATH=../../../tools/:$PYHONPATH; 
	python -c "import sys, run; print run.WaterOutputAbstraction(0, sys.argv[1], '')" "$stdout" > seq-out-${NP}
#	awk -f get-output.awk > SEQ-OUT.${NP}
    done
}


function run_par #(numpoints, repetitions, threads)#
{
    NT=$3
    NP=$1
    REP=$2
    rm -f LWI5
    rm -f PAR-OUT.${NP}.${NT}

    export PL_SCHED_MODE=0
    export PL_THREAD_NUM=$NT
    for i in `seq 1 ${REP}`; do
	./water-par ../inputs/LWI5.$NP random.in > SEQ-TMP-OUT.${i};
	local stdout=`cat SEQ-TMP-OUT.${i}`
	rm -f SEQ-TMP-OUT-${i}
	
	export PYTHONPATH=../../../tools/:$PYHONPATH; 
	python -c "import sys, run; print run.WaterOutputAbstraction(0, sys.argv[1], '')" "$stdout" > par-aout-${NP}.${i}

	mv exectime.txt par-dtime.${NP}.${NT}.${i}
    done

}

source "${BASE}/run.base.e.sh"

