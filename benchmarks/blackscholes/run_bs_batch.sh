#!/bin/bash

INP=16K
TYP=static
PROB=0.01
BLOCKSIZE=2
#STARTSEED=1
#NSEEDS=3
STARTSEED=7
NSEEDS=9
FLAGS=

echo "blacscholes is beginning execution." > tmp.txt
echo "flags used: $FLAGS" >> tmp.txt
echo "seeds: $STARTSEED -> $NSEEDS" >> tmp.txt
echo "input: $INP" >> tmp.txt

#mutt -s "Blackscholes Started" sarachour@gmail.com < tmp.txt


for i in $(seq $STARTSEED $NSEEDS);
do

#sutil_run_script.sh run_bs.sh $INP $i reexec:t=scar,b=5,p=$PROB iact-med-$TYP $FLAGS
#sutil_run_script.sh run_bs.sh $INP $i reexec:t=scar,b=4,p=$PROB iact-med-$TYP $FLAGS
#sutil_run_script.sh run_bs.sh $INP $i reexec:t=scar,b=3,p=$PROB iact-med-$TYP $FLAGS
#sutil_run_script.sh run_bs.sh $INP $i reexec:t=scar,b=2,p=$PROB iact-med-$TYP $FLAGS

#sutil_run_script.sh run_bs.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.0 iact-med-$TYP $FLAGS
#sutil_run_script.sh run_bs.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.01 iact-med-$TYP $FLAGS
#sutil_run_script.sh run_bs.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.02 iact-med-$TYP $FLAGS
#sutil_run_script.sh run_bs.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.04 iact-med-$TYP $FLAGS
#sutil_run_script.sh run_bs.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.08 iact-med-$TYP $FLAGS

sutil_run_script.sh run_bs.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.00 iact-med-$TYP $FLAGS
sutil_run_script.sh run_bs.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.01 iact-med-$TYP $FLAGS
sutil_run_script.sh run_bs.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.0001 iact-med-$TYP $FLAGS
#sutil_run_script.sh run_bs.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.40 iact-med-$TYP $FLAGS
#sutil_run_script.sh run_bs.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.60 iact-med-$TYP $FLAGS
#sutil_run_script.sh run_bs.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.80 iact-med-$TYP $FLAGS
#sutil_run_script.sh run_bs.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=1.00 iact-med-$TYP $FLAGS

done


echo "blacscholes is finished executing." > tmp.txt
echo "flags used: $FLAGS" >> tmp.txt
echo "seeds: $STARTSEED -> $NSEEDS" >> tmp.txt
echo "input: $INP" >> tmp.txt

mutt -s "Blackscholes Finished" sarachour@gmail.com  < tmp.txt
