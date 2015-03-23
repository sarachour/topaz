#!/bin/bash


function baseCreateDirectories {
	outdir=$1
	if [ ! -d output ]; then
		mkdir output
	fi
	if [ ! -d output/$outdir ]; then
		mkdir output/$outdir
	fi
	if [ ! -d profile ]; then
		mkdir profile
	fi
	if [ ! -d logs ]; then
		mkdir logs
	fi
}

function baseUpdateDirectories {
	outdir=$1
	suff=$2
	if [ ! -d output ]; then
		mkdir output
	fi
	if [ ! -d output/$outdir ]; then
		mkdir output/$outdir 
	fi
	if [ ! -d output/$outdir/timers.$suff ]; then
		mkdir output/$outdir/timers.$suff
	fi
	if [ -d output/$outdir/data.$suff ]; then
		rm -rf output/$outdir/data.$suff
	fi
	if [ -d profile ]; then
		rm -rf output/$outdir/profile.$suff
		mv profile output/$outdir/profile.$suff
	fi
	if [ -d logs ]; then
		mv logs output/$outdir/data.$suff
	else
		mkdir output/$outdir/data.$suff
	fi
	mv timer.*.out output/$outdir/timers.$suff/
	mv task.*.out output/$outdir/data.$suff/
	mv ldet.out output/$outdir/data.$suff/
	mv comm.*.out output/$outdir/timers.$suff/
}
