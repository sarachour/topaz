#!/bin/bash

BASE=$(basename $1 .ppm)
BASEPATH=$(dirname $1)
CORRECT=output/correct.*/$BASE.ppm
DIFFFILE=$BASEPATH/$BASE.diff.ppm
echo compare -metric PSNR $CORRECT $1 $DIFFFILE
compare -metric PSNR $CORRECT $1 $DIFFFILE
