#!/bin/bash

INP=1
TYP=static
PROB=0.01
BLOCKSIZE=2
STARTSEED=8
NSEEDS=8
#STARTSEED=6
#NSEEDS=7
SCRIPT=run_body

FLAGS=t

echo "bodytrack has started execution." > tmp.txt
echo "flags used: $FLAGS" >> tmp.txt
echo "seeds: $STARTSEED -> $NSEEDS" >> tmp.txt
echo "input: $INP" >> tmp.txt

#mutt -s "Bodytrack Started" sarachour@gmail.com < tmp.txt

sutil_run_script.sh $SCRIPT.sh $INP $i perfect none
for i in $(seq $STARTSEED $NSEEDS);
do

#sutil_run_script.sh $SCRIPT.sh $INP $i reexec:t=scar,b=5,p=$PROB iact-med-$TYP $FLAGS
#sutil_run_script.sh $SCRIPT.sh $INP $i reexec:t=scar,b=4,p=$PROB iact-med-$TYP $FLAGS
#sutil_run_script.sh $SCRIPT.sh $INP $i reexec:t=scar,b=3,p=$PROB iact-med-$TYP $FLAGS
#sutil_run_script.sh $SCRIPT.sh $INP $i reexec:t=scar,b=2,p=$PROB iact-med-$TYP $FLAGS

#sutil_run_script.sh $SCRIPT.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.0 iact-med-$TYP $FLAGS
#sutil_run_script.sh $SCRIPT.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.01 iact-med-$TYP $FLAGS
#sutil_run_script.sh $SCRIPT.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.02 iact-med-$TYP $FLAGS
#sutil_run_script.sh $SCRIPT.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.04 iact-med-$TYP $FLAGS
#sutil_run_script.sh $SCRIPT.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.08 iact-med-$TYP $FLAGS

sutil_run_script.sh $SCRIPT.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.00 iact-med-$TYP $FLAGS
#sutil_run_script.sh $SCRIPT.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.01 iact-med-$TYP $FLAGS
#sutil_run_script.sh $SCRIPT.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.0001 iact-med-$TYP $FLAGS
#sutil_run_script.sh $SCRIPT.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.10 iact-med-$TYP $FLAGS
#sutil_run_script.sh $SCRIPT.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.20 iact-med-$TYP $FLAGS
#sutil_run_script.sh $SCRIPT.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.40 iact-med-$TYP $FLAGS
#sutil_run_script.sh $SCRIPT.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.60 iact-med-$TYP $FLAGS
#sutil_run_script.sh $SCRIPT.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.80 iact-med-$TYP $FLAGS
#sutil_run_script.sh $SCRIPT.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=1.00 iact-med-$TYP $FLAGS


done

echo "bodytrack is finished executing." > tmp.txt
echo "flags used: $FLAGS" >> tmp.txt
echo "seeds: $STARTSEED -> $NSEEDS" >> tmp.txt
echo "input: $INP" >> tmp.txt

mutt -s "Bodytrack Finished" sarachour@gmail.com < tmp.txt
