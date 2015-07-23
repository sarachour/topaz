#!/bin/bash

INP=16K
TYP=static2
PROB=0.01
BLOCKSIZE=2


# usually 7
STARTSEED=7
NSEEDS=7
FLAGS=d

echo "blacscholes is beginning execution." > tmp.txt
echo "flags used: $FLAGS" >> tmp.txt
echo "seeds: $STARTSEED -> $NSEEDS" >> tmp.txt
echo "input: $INP" >> tmp.txt

#mutt -s "Blackscholes Started" sarachour@gmail.com < tmp.txt


sutil_run_script.sh run_bs.sh $INP 1 none perfect
for i in $(seq $STARTSEED $NSEEDS);
do


#sutil_run_script.sh run_bs.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.00 iact-med-$TYP t
#sutil_run_script.sh run_bs.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.00 iact-med-$TYP d


FLAGS=t
#sutil_run_script.sh run_bs.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.01 iact-med-$TYP $FLAGS
#sutil_run_script.sh run_bs.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.02 iact-med-$TYP $FLAGS
#sutil_run_script.sh run_bs.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.04 iact-med-$TYP $FLAGS
#sutil_run_script.sh run_bs.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.08 iact-med-$TYP $FLAGS
sutil_run_script.sh run_bs.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.11 iact-med-$TYP $FLAGS
sutil_run_script.sh run_bs.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.16 iact-med-$TYP $FLAGS

FLAGS=d
#sutil_run_script.sh run_bs.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.01 iact-med-$TYP $FLAGS
#sutil_run_script.sh run_bs.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.02 iact-med-$TYP $FLAGS
#sutil_run_script.sh run_bs.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.04 iact-med-$TYP $FLAGS
#sutil_run_script.sh run_bs.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.08 iact-med-$TYP $FLAGS
sutil_run_script.sh run_bs.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.11 iact-med-$TYP $FLAGS
#sutil_run_script.sh run_bs.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.16 iact-med-$TYP $FLAGS



done


echo "blacscholes is finished executing." > tmp.txt
echo "flags used: $FLAGS" >> tmp.txt
echo "seeds: $STARTSEED -> $NSEEDS" >> tmp.txt
echo "input: $INP" >> tmp.txt

mutt -s "Blackscholes Finished" sarachour@gmail.com  < tmp.txt
