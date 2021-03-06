INPUT=16K
SEED=2
TYP=med-static
STARTSEED=1
NSEEDS=1

#echo "aov batch is beginning execution." > tmp.txt
#echo "flags $FLAGS" >> tmp.txt
#mutt -s "AOV Started" sarachour@gmail.com < tmp.txt

#./run_aov.sh correct none $INPUT 1 || exit 1
echo "starting run"

RATE=0.00
for SEED in $(seq $STARTSEED $NSEEDS);
do
	echo "seed $SEED"
	
	FLAGS=t
	./run_aov.sh batching batch1 $INPUT $SEED reexec:t=scar,b=2,p=$RATE iact-$TYP $FLAGS || exit 1
	./run_aov.sh batching batch2 $INPUT $SEED reexec:t=scar,b=2,p=$RATE iact-$TYP $FLAGS || exit 1
	./run_aov.sh batching batch4 $INPUT $SEED reexec:t=scar,b=2,p=$RATE iact-$TYP $FLAGS || exit 1
	./run_aov.sh batching batch8 $INPUT $SEED reexec:t=scar,b=2,p=$RATE iact-$TYP $FLAGS || exit 1
	
	FLAGS=d
	./run_aov.sh batching batch1 $INPUT $SEED reexec:t=scar,b=2,p=$RATE iact-$TYP $FLAGS || exit 1
	./run_aov.sh batching batch2 $INPUT $SEED reexec:t=scar,b=2,p=$RATE iact-$TYP $FLAGS || exit 1
	./run_aov.sh batching batch4 $INPUT $SEED reexec:t=scar,b=2,p=$RATE iact-$TYP $FLAGS || exit 1
	./run_aov.sh batching batch8 $INPUT $SEED reexec:t=scar,b=2,p=$RATE iact-$TYP $FLAGS || exit 1
	
	FLAGS=t
	#./run_aov.sh normalization naive $INPUT $SEED reexec:t=scar,b=2,p=$RATE iact-$TYP $FLAGS || exit 1
	#./run_aov.sh normalization arbitrarge $INPUT $SEED reexec:t=scar,b=2,p=$RATE iact-$TYP $FLAGS || exit 1
	
	FLAGS=d
	#./run_aov.sh normalization naive $INPUT $SEED reexec:t=scar,b=2,p=$RATE iact-$TYP $FLAGS || exit 1
	#./run_aov.sh normalization arbitrarge $INPUT $SEED reexec:t=scar,b=2,p=$RATE iact-$TYP $FLAGS || exit 1
	
	
	FLAGS=t
	#./run_aov.sh selection out $INPUT $SEED reexec:t=scar,b=2,p=$RATE iact-$TYP $FLAGS || exit 1
	#./run_aov.sh selection inout $INPUT $SEED reexec:t=scar,b=2,p=$RATE iact-$TYP $FLAGS || exit 1
	#./run_aov.sh selection all $INPUT $SEED reexec:t=scar,b=2,p=$RATE iact-$TYP $FLAGS || exit 1
	#./run_aov.sh selection strikeout $INPUT $SEED reexec:t=scar,b=2,p=$RATE iact-$TYP $FLAGS || exit 1
	#./run_aov.sh selection timeout $INPUT $SEED reexec:t=scar,b=2,p=$RATE iact-$TYP $FLAGS || exit 1
	#./run_aov.sh selection rateout $INPUT $SEED reexec:t=scar,b=2,p=$RATE iact-$TYP $FLAGS || exit 1
	#./run_aov.sh selection typeout $INPUT $SEED reexec:t=scar,b=2,p=$RATE iact-$TYP $FLAGS || exit 1

	FLAGS=d
	#./run_aov.sh selection out $INPUT $SEED reexec:t=scar,b=2,p=$RATE iact-$TYP $FLAGS || exit 1
	#./run_aov.sh selection inout $INPUT $SEED reexec:t=scar,b=2,p=$RATE iact-$TYP $FLAGS || exit 1
	#./run_aov.sh selection all $INPUT $SEED reexec:t=scar,b=2,p=$RATE iact-$TYP $FLAGS || exit 1
	#./run_aov.sh selection strikeout $INPUT $SEED reexec:t=scar,b=2,p=$RATE iact-$TYP $FLAGS || exit 1
	#./run_aov.sh selection timeout $INPUT $SEED reexec:t=scar,b=2,p=$RATE iact-$TYP $FLAGS || exit 1
	#./run_aov.sh selection rateout $INPUT $SEED reexec:t=scar,b=2,p=$RATE iact-$TYP $FLAGS || exit 1
	#./run_aov.sh selection typeout $INPUT $SEED reexec:t=scar,b=2,p=$RATE iact-$TYP $FLAGS || exit 1


done

echo "aov experiment finished." > tmp.txt
echo "flags $FLAGS" >> tmp.txt
mutt -s "AOV Finished" sarachour@gmail.com < tmp.txt
