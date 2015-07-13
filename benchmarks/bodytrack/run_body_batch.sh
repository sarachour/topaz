#!/bin/bash

INP=1
TYP=static
PROB=0.01
BLOCKSIZE=5
#STARTSEED=1
#NSEEDS=3
STARTSEED=1
NSEEDS=8
SCRIPT=run_body

FLAGS=

for i in $(seq $STARTSEED $NSEEDS);
do

sutil_run_script.sh $SCRIPT.sh $INP $i reexec:t=scar,b=5,p=$PROB iact-med-$TYP $FLAGS
sutil_run_script.sh $SCRIPT.sh $INP $i reexec:t=scar,b=4,p=$PROB iact-med-$TYP $FLAGS
sutil_run_script.sh $SCRIPT.sh $INP $i reexec:t=scar,b=3,p=$PROB iact-med-$TYP $FLAGS
sutil_run_script.sh $SCRIPT.sh $INP $i reexec:t=scar,b=2,p=$PROB iact-med-$TYP $FLAGS

sutil_run_script.sh $SCRIPT.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.0 iact-med-$TYP $FLAGS
sutil_run_script.sh $SCRIPT.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.01 iact-med-$TYP $FLAGS
sutil_run_script.sh $SCRIPT.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.02 iact-med-$TYP $FLAGS
sutil_run_script.sh $SCRIPT.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.04 iact-med-$TYP $FLAGS
sutil_run_script.sh $SCRIPT.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.08 iact-med-$TYP $FLAGS


done

echo "bodytrack is finished executing." > tmp.txt
echo "flags used: $FLAGS" >> tmp.txt
echo "seeds: $STARTSEED -> $NSEEDS" >> tmp.txt
echo "input: $INP" >> tmp.txt

mutt -s "Bodytrack Finished" sachour@mit.edu < tmp.txt
