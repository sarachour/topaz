#!/bin/bash

INP=m100t12r2
TYP=dynamic
PROB=0.01
BS=2
STARTSEED=1
NSEEDS=1
#STARTSEED=4
#NSEEDS=10
FLAGS=t

echo "water has started executing." > tmp.txt
echo "flags used: $FLAGS" >> tmp.txt
echo "seeds: $STARTSEED -> $NSEEDS" >> tmp.txt
echo "input: $INP" >> tmp.txt

#mutt -s "Water Started" sarachour@gmail.com  < tmp.txt

sutil_run_script.sh run_water.sh $INP perfect none
for i in $(seq $STARTSEED $NSEEDS);
do

sutil_run_script.sh run_water.sh $INP $i reexec:t=scar,b=$BS,p=0.00 iact-med-$TYP t
sutil_run_script.sh run_water.sh $INP $i reexec:t=scar,b=$BS,p=0.00 iact-med-$TYP d

FLAGS=t
sutil_run_script.sh run_water.sh $INP $i reexec:t=scar,b=$BS,p=0.01 iact-med-$TYP $FLAGS
sutil_run_script.sh run_water.sh $INP $i reexec:t=scar,b=$BS,p=0.02 iact-med-$TYP $FLAGS
sutil_run_script.sh run_water.sh $INP $i reexec:t=scar,b=$BS,p=0.04 iact-med-$TYP $FLAGS
#sutil_run_script.sh run_water.sh $INP $i reexec:t=scar,b=$BS,p=0.08 iact-med-$TYP $FLAGS
#sutil_run_script.sh run_water.sh $INP $i reexec:t=scar,b=$BS,p=0.11 iact-med-$TYP $FLAGS
#sutil_run_script.sh run_water.sh $INP $i reexec:t=scar,b=$BS,p=0.16 iact-med-$TYP $FLAGS

FLAGS=d
sutil_run_script.sh run_water.sh $INP $i reexec:t=scar,b=$BS,p=0.01 iact-med-$TYP $FLAGS
sutil_run_script.sh run_water.sh $INP $i reexec:t=scar,b=$BS,p=0.02 iact-med-$TYP $FLAGS
sutil_run_script.sh run_water.sh $INP $i reexec:t=scar,b=$BS,p=0.04 iact-med-$TYP $FLAGS
#sutil_run_script.sh run_water.sh $INP $i reexec:t=scar,b=$BS,p=0.08 iact-med-$TYP $FLAGS
#sutil_run_script.sh run_water.sh $INP $i reexec:t=scar,b=$BS,p=0.11 iact-med-$TYP $FLAGS
#sutil_run_script.sh run_water.sh $INP $i reexec:t=scar,b=$BS,p=0.16 iact-med-$TYP $FLAGS




done

echo "water is finished executing." > tmp.txt
echo "flags used: $FLAGS" >> tmp.txt
echo "seeds: $STARTSEED -> $NSEEDS" >> tmp.txt
echo "input: $INP" >> tmp.txt

mutt -s "Water Finished" sarachour@gmail.com  < tmp.txt
