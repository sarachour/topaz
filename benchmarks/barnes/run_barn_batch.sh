#!/bin/bash

INP=4K
TYP=static
PROB=0.01
BLOCKSIZE=2
#STARTSEED=1
#NSEEDS=3
STARTSEED=8
NSEEDS=8
SCRIPT=run_barn

FLAGS=d

echo "barnes has started executing." > tmp.txt
echo "flags used: $FLAGS" >> tmp.txt
echo "seeds: $STARTSEED -> $NSEEDS" >> tmp.txt
echo "input: $INP" >> tmp.txt

mutt -s "Barnes Started" sarachour@gmail.com  < tmp.txt

for i in $(seq $STARTSEED $NSEEDS);
do

#sutil_run_script.sh $SCRIPT.sh $INP $i reexec:t=scar,b=5,p=$PROB iact-med-$TYP $FLAGS
#sutil_run_script.sh $SCRIPT.sh $INP $i reexec:t=scar,b=4,p=$PROB iact-med-$TYP $FLAGS
#sutil_run_script.sh $SCRIPT.sh $INP $i reexec:t=scar,b=3,p=$PROB iact-med-$TYP $FLAGS
#sutil_run_script.sh $SCRIPT.sh $INP $i reexec:t=scar,b=2,p=$PROB iact-med-$TYP $FLAGS




#sutil_run_script.sh $SCRIPT.sh $INP $i none perfect
#sutil_run_script.sh $SCRIPT.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.00 iact-med-$TYP $FLAGS
#sutil_run_script.sh $SCRIPT.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.00 iact-med-$TYP d
sutil_run_script.sh $SCRIPT.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.01 iact-med-$TYP $FLAGS
sutil_run_script.sh $SCRIPT.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.02 iact-med-$TYP $FLAGS
sutil_run_script.sh $SCRIPT.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.04 iact-med-$TYP $FLAGS
sutil_run_script.sh $SCRIPT.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.08 iact-med-$TYP $FLAGS
sutil_run_script.sh $SCRIPT.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.11 iact-med-$TYP $FLAGS
sutil_run_script.sh $SCRIPT.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.16 iact-med-$TYP $FLAGS

#sutil_run_script.sh $SCRIPT.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.10 iact-med-$TYP $FLAGS
#sutil_run_script.sh $SCRIPT.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.20 iact-med-$TYP $FLAGS
#sutil_run_script.sh $SCRIPT.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.40 iact-med-$TYP $FLAGS
#sutil_run_script.sh $SCRIPT.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.60 iact-med-$TYP $FLAGS
#sutil_run_script.sh $SCRIPT.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.80 iact-med-$TYP $FLAGS
#sutil_run_script.sh $SCRIPT.sh $INP $i reexec:t=scar,b=$BLOCKSIZE,p=0.00 iact-med-$TYP $FLAGS


done


echo "barnes is finished executing." > tmp.txt
echo "flags used: $FLAGS" >> tmp.txt
echo "seeds: $STARTSEED -> $NSEEDS" >> tmp.txt
echo "input: $INP" >> tmp.txt

mutt -s "Barnes Finished" sarachour@gmail.com  < tmp.txt
