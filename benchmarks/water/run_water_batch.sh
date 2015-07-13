#!/bin/bash

INP=m100t12r2
TYP=static
PROB=0.01
BS=5
#STARTSEED=1
#NSEEDS=3
STARTSEED=4
NSEEDS=10
FLAGS=

for i in $(seq $STARTSEED $NSEEDS);
do

sutil_run_script.sh run_water.sh $INP $i reexec:t=scar,b=5,p=$PROB iact-med-$TYP $FLAGS
sutil_run_script.sh run_water.sh $INP $i reexec:t=scar,b=4,p=$PROB iact-med-$TYP $FLAGS
sutil_run_script.sh run_water.sh $INP $i reexec:t=scar,b=3,p=$PROB iact-med-$TYP $FLAGS
sutil_run_script.sh run_water.sh $INP $i reexec:t=scar,b=2,p=$PROB iact-med-$TYP $FLAGS

sutil_run_script.sh run_water.sh $INP $i reexec:t=scar,b=$BS,p=0.0 iact-med-$TYP $FLAGS
sutil_run_script.sh run_water.sh $INP $i reexec:t=scar,b=$BS,p=0.01 iact-med-$TYP $FLAGS
sutil_run_script.sh run_water.sh $INP $i reexec:t=scar,b=$BS,p=0.02 iact-med-$TYP $FLAGS
sutil_run_script.sh run_water.sh $INP $i reexec:t=scar,b=$BS,p=0.04 iact-med-$TYP $FLAGS


done
