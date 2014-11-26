#!/bin/bash

F=Makefile.llvm
OUT=`cat Makefile.llvm | grep "OUT=" | awk -F '/' '{printf ("%s/%s\n", $(NF-1), $NF) }'`

echo "Makefile configuration: $OUT"

cd $OUT
rm -f water-*
cd -

make -f $F clean;
make -f $F || exit 1
mv $OUT/water $OUT/water-seq

make -f $F clean
make -f $F EXTRAFLAGS="-D_USE_PARALLEL_ -D_PAR_INTERF_ -D_PAR_POTENG_" || exit 1
mv $OUT/water $OUT/water-parunsync

make -f $F clean
make -f $F EXTRAFLAGS="-D_USE_PARALLEL_ -D_PAR_INTERF_" || exit 1
mv  $OUT/water $OUT/water-pinterf

make -f $F clean
make -f $F EXTRAFLAGS="-D_USE_PARALLEL_ -D_PAR_INTERF_ -D_SYNC_VIR_" || exit 1
mv  $OUT/water  $OUT/water-pinterfsyncvir

make -f $F clean
make -f $F EXTRAFLAGS="-D_USE_PARALLEL_ -D_PAR_INTERF_ -D_SYNC_INTERF_" || exit 1
mv  $OUT/water  $OUT/water-pinterfsyncmol

make -f $F clean
make -f $F EXTRAFLAGS="-D_USE_PARALLEL_ -D_PAR_INTERF_ -D_SYNC_INTERF_ -D_SYNC_VIR_" || exit 1
mv  $OUT/water  $OUT/water-pinterfsyncall

make -f $F clean
make EXTRAFLAGS="-D_USE_PARALLEL_ -D_PAR_INTERF_ -D_SYNC_INTERF_ -D_PAR_REPL_ACCUMULATOR_INT_" || exit 1
mv  $OUT/water  $OUT/water-pinterffinal

make -f $F clean
make -f $F EXTRAFLAGS="-D_USE_PARALLEL_ -D_PAR_INTERF_ -D_SYNC_INTERF_ -D_SYNC_VIR_ -D_PAR_POTENG_ -D_SYNC_POTENG_"  || exit 1
mv  $OUT/water  $OUT/water-psyncall

make -f $F clean
make -f $F EXTRAFLAGS="-D_USE_PARALLEL_ -D_PAR_INTERF_ -D_REPL_ACCUMULATOR_INT_"  || exit 1
mv  $OUT/water  $OUT/water-pinterfrepl

make -f $F clean
make -f $F EXTRAFLAGS="-D_USE_PARALLEL_ -D_PAR_INTERF_ -D_REPL_ACCUMULATOR_INT_ -D_PAR_POTENG_"  || exit 1
mv  $OUT/water  $OUT/water-pvirreplpotunsync

make -f $F clean
make -f $F EXTRAFLAGS="-D_USE_PARALLEL_ -D_PAR_INTERF_ -D_REPL_ACCUMULATOR_INT_ -D_PAR_POTENG_ -D_SYNC_POTENG_"  || exit 1
mv  $OUT/water  $OUT/water-pvirreplpotsync

make -f $F clean
make -f $F EXTRAFLAGS="-D_USE_PARALLEL_ -D_PAR_INTERF_ -D_REPL_ACCUMULATOR_INT_ -D_REPL_ACCUMULATOR_POT_  -D_PAR_POTENG_"   || exit 1
mv  $OUT/water  $OUT/water-preplall

make -f $F clean
make -f $F EXTRAFLAGS="-D_USE_PARALLEL_ -D_PAR_INTERF_ -D_SYNC_INTERF_ -D_REPL_ACCUMULATOR_POT_ -D_REPL_ACCUMULATOR_INT_ -D_PAR_POTENG_"   || exit 1
mv  $OUT/water  $OUT/water-pfinal

