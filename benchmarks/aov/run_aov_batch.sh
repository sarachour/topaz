INPUT=16K
SEED=1
TYP=heavy-static
FLAGS=

#echo "aov batch is beginning execution." > tmp.txt
#echo "flags $FLAGS" >> tmp.txt
#mutt -s "AOV Started" sarachour@gmail.com < tmp.txt

./run_aov.sh correct none $INPUT $SEED || exit 1

./run_aov.sh normalization naive $INPUT $SEED reexec:t=scar,b=5,p=0.01 iact-$TYP $FLAGS || exit 1
./run_aov.sh normalization subtract $INPUT $SEED reexec:t=scar,b=5,p=0.01 iact-$TYP $FLAGS || exit 1
./run_aov.sh normalization arbitrarge $INPUT $SEED reexec:t=scar,b=5,p=0.01 iact-$TYP $FLAGS || exit 1

#echo "aov batch is moving onto batching tasks." > tmp.txt
#echo "flags $FLAGS" >> tmp.txt
#mutt -s "AOV on Batching" sarachour@gmail.com < tmp.txt


./run_aov.sh batching batch1 $INPUT $SEED reexec:t=scar,b=5,p=0.01 iact-$TYP $FLAGS || exit 1
./run_aov.sh batching batch2 $INPUT $SEED reexec:t=scar,b=5,p=0.01 iact-$TYP $FLAGS || exit 1
./run_aov.sh batching batch4 $INPUT $SEED reexec:t=scar,b=5,p=0.01 iact-$TYP $FLAGS || exit 1
./run_aov.sh batching batch8 $INPUT $SEED reexec:t=scar,b=5,p=0.01 iact-$TYP $FLAGS || exit 1




./run_aov.sh selection out $INPUT $SEED reexec:t=scar,b=5,p=0.01 iact-$TYP $FLAGS || exit 1
./run_aov.sh selection inout $INPUT $SEED reexec:t=scar,b=5,p=0.01 iact-$TYP $FLAGS || exit 1
./run_aov.sh selection all $INPUT $SEED reexec:t=scar,b=5,p=0.01 iact-$TYP $FLAGS || exit 1

echo "aov experiment finished." > tmp.txt
echo "flags $FLAGS" >> tmp.txt
mutt -s "AOV Finished" sarachour@gmail.com < tmp.txt
