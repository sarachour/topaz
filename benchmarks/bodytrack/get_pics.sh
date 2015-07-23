#!/bin/bash

PARDIR=output-batch5
mkdir -p pics
rm -rf pics/*

cp tradeoff_curve*.png pics/
cp correl_*.png pics/

for file in $PARDIR/iact-med-static.reexec.scar.p0.**.b2.ldet/data*/*.region.*.png; 
do
	PROB=$(echo $file | grep -o -E "p[0-9\.]+")
	FILE=$(echo $file | grep -o -E "[^/]+$")
	cp $file pics/$PROB$FILE
done
