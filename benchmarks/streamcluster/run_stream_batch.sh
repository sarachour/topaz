#!/bin/bash

INP=500
TYP=static
PROB=0.01
BLOCKSIZE=2
# 6 works with time, 7 for detection
STARTSEED=7
NSEEDS=7
SCRIPT=run_stream

FLAGS=d

echo "streamcluster has started executing." > tmp.txt
echo "flags used: $FLAGS" >> tmp.txt
echo "seeds: $STARTSEED -> $NSEEDS" >> tmp.txt
echo "input: $INP" >> tmp.txt

#mutt -s "Streamcluster Started" sarachour@gmail.com  < tmp.txt


#sutil_run_script.sh $SCRIPT.sh $INP 1 none perfect 
#sutil_run_script.sh $SCRIPT.sh $INP 1 reexec:t=scar,b=$BLOCKSIZE,p=0.00 perfect d


for i in $(seq $STARTSEED $NSEEDS);
do

#SEED=6
FLAGS=t
sutil_run_script.sh $SCRIPT.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.00 iact-med-$TYP $FLAGS


#SEED=7
#FLAGS=d
#sutil_run_script.sh $SCRIPT.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.00 iact-med-$TYP $FLAGS


#tried 6,7,8,4
#SEED=5
#FLAGS=d
#sutil_run_script.sh $SCRIPT.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.01 iact-med-$TYP $FLAGS
#sutil_run_script.sh $SCRIPT.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.02 iact-med-$TYP $FLAGS
#sutil_run_script.sh $SCRIPT.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.04 iact-med-$TYP $FLAGS
#sutil_run_script.sh $SCRIPT.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.08 iact-med-$TYP $FLAGS
#sutil_run_script.sh $SCRIPT.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.11 iact-med-$TYP $FLAGS
#sutil_run_script.sh $SCRIPT.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.16 iact-med-$TYP $FLAGS

#FLAGS=t
#sutil_run_script.sh $SCRIPT.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.00 iact-med-$TYP $FLAGS
#sutil_run_script.sh $SCRIPT.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.01 iact-med-$TYP $FLAGS
#sutil_run_script.sh $SCRIPT.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.02 iact-med-$TYP $FLAGS
#sutil_run_script.sh $SCRIPT.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.04 iact-med-$TYP $FLAGS
#sutil_run_script.sh $SCRIPT.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.08 iact-med-$TYP $FLAGS
#sutil_run_script.sh $SCRIPT.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.11 iact-med-$TYP $FLAGS
#sutil_run_script.sh $SCRIPT.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.16 iact-med-$TYP $FLAGS


done


#echo "streamcluster is finished executing." > tmp.txt
#echo "flags used: $FLAGS" >> tmp.txt
#echo "seeds: $STARTSEED -> $NSEEDS" >> tmp.txt
#echo "input: $INP" >> tmp.txt
#mutt -s "Streamcluster Finished" sarachour@gmail.com  < tmp.txt
