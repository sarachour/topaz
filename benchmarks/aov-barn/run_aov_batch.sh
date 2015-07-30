INPUT=1K
SEED=1
TYP=med-static
STARTSEED=1
NSEEDS=1

#echo "aov batch is beginning execution." > tmp.txt
#echo "flags $FLAGS" >> tmp.txt
#mutt -s "AOV Started" sarachour@gmail.com < tmp.txt

#./run_aov.sh correct none $INPUT 1 || exit 1
echo "starting run"

for SEED in $(seq $STARTSEED $NSEEDS);
do
	FLAGS=d
	echo "seed $SEED"
	#./run_aov.sh batching batch1 $INPUT $SEED reexec:t=scar,b=2,p=0.00 iact-$TYP $FLAGS || exit 1
	#./run_aov.sh batching batch2 $INPUT $SEED reexec:t=scar,b=2,p=0.00 iact-$TYP $FLAGS || exit 1
	
	FLAGS=t
	echo "seed $SEED"
	./run_aov.sh batching batch1 $INPUT $SEED reexec:t=scar,b=2,p=0.00 iact-$TYP $FLAGS || exit 1
	./run_aov.sh batching batch2 $INPUT $SEED reexec:t=scar,b=2,p=0.00 iact-$TYP $FLAGS || exit 1
	


done

echo "aov experiment finished." > tmp.txt
echo "flags $FLAGS" >> tmp.txt
mutt -s "AOV Finished" sarachour@gmail.com < tmp.txt
