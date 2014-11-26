#!/bin/bash

F=Makefile.llvm
##HACK:
OUT=`cat $F | grep "OUT=" | awk -F '/' '{printf ("%s/%s\n", $(NF-1), $NF) }'`

echo "Makefile configuration: $OUT"

make -f $F clean;
make -f $F
mv $OUT/water $OUT/water-seq

make -f $F clean
make -f $F EXTRAFLAGS="-D_USE_PARALLEL_ -D_PAR_INTERF_ -D_PAR_POTENG_"
mv $OUT/water $OUT/water-parunsync

make -f $F clean
make -f $F EXTRAFLAGS="-D_USE_PARALLEL_ -D_PAR_INTERF_ -D_SYNC_INTERF_ -D_SYNC_VIR_ -D_PAR_POTENG_ -D_SYNC_POTENG_"
mv $OUT/water $OUT/water-psyncall

make -f $F clean
make -f $F EXTRAFLAGS="-D_USE_PARALLEL_ -D_PAR_INTERF_ -D_REPL_ACCUMULATOR_INT_ -D_REPL_ACCUMULATOR_POT_  -D_PAR_POTENG_" 
mv $OUT/water $OUT/water-preplall

make -f $F clean
make -f $F EXTRAFLAGS="-D_USE_PARALLEL_ -D_PAR_INTERF_ -D_SYNC_INTERF_ -D_REPL_ACCUMULATOR_POT_ -D_REPL_ACCUMULATOR_INT_ -D_PAR_POTENG_" 
mv $OUT/water $OUT/water-pfinal

