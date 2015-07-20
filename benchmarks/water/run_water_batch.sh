#!/bin/bash

INP=m100t12r2
TYP=static
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

mutt -s "Water Started" sarachour@gmail.com  < tmp.txt

for i in $(seq $STARTSEED $NSEEDS);
do

#sutil_run_script.sh run_water.sh $INP $i reexec:t=scar,b=5,p=$PROB iact-med-$TYP $FLAGS
#sutil_run_script.sh run_water.sh $INP $i reexec:t=scar,b=4,p=$PROB iact-med-$TYP $FLAGS
#sutil_run_script.sh run_water.sh $INP $i reexec:t=scar,b=3,p=$PROB iact-med-$TYP $FLAGS
#sutil_run_script.sh run_water.sh $INP $i reexec:t=scar,b=2,p=$PROB iact-med-$TYP $FLAGS

#sutil_run_script.sh run_water.sh $INP $i reexec:t=scar,b=$BS,p=0.0 iact-med-$TYP $FLAGS
#sutil_run_script.sh run_water.sh $INP $i reexec:t=scar,b=$BS,p=0.01 iact-med-$TYP $FLAGS
#sutil_run_script.sh run_water.sh $INP $i reexec:t=scar,b=$BS,p=0.02 iact-med-$TYP $FLAGS
#sutil_run_script.sh run_water.sh $INP $i reexec:t=scar,b=$BS,p=0.04 iact-med-$TYP $FLAGS

#sutil_run_script.sh run_water.sh $INP $i none perfect
#sutil_run_script.sh run_water.sh $INP $i reexec:t=scar,b=$BS,p=0.00 iact-med-$TYP $FLAGS
#sutil_run_script.sh run_water.sh $INP $i reexec:t=scar,b=$BS,p=0.01 iact-med-$TYP $FLAGS
sutil_run_script.sh run_water.sh $INP $i reexec:t=scar,b=$BS,p=0.08 iact-med-$TYP $FLAGS

#sutil_run_script.sh run_water.sh $INP $i reexec:t=scar,b=$BS,p=0.10 iact-med-$TYP $FLAGS
#sutil_run_script.sh run_water.sh $INP $i reexec:t=scar,b=$BS,p=0.20 iact-med-$TYP $FLAGS
#sutil_run_script.sh run_water.sh $INP $i reexec:t=scar,b=$BS,p=0.40 iact-med-$TYP $FLAGS
#sutil_run_script.sh run_water.sh $INP $i reexec:t=scar,b=$BS,p=0.60 iact-med-$TYP $FLAGS
#sutil_run_script.sh run_water.sh $INP $i reexec:t=scar,b=$BS,p=0.80 iact-med-$TYP $FLAGS
#sutil_run_script.sh run_water.sh $INP $i reexec:t=scar,b=$BS,p=1.00 iact-med-$TYP $FLAGS

done

echo "water is finished executing." > tmp.txt
echo "flags used: $FLAGS" >> tmp.txt
echo "seeds: $STARTSEED -> $NSEEDS" >> tmp.txt
echo "input: $INP" >> tmp.txt

mutt -s "Water Finished" sarachour@gmail.com  < tmp.txt
