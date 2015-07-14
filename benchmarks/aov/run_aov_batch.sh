INPUT=16K
SEED=1
TYP=heavy-static
FLAGS=


#./run_aov.sh correct none $INPUT $SEED
./run_aov.sh normalization naive $INPUT $SEED reexec:t=scar,b=5,p=0.01 iact-$TYP $FLAGS
./run_aov.sh normalization subtract $INPUT $SEED reexec:t=scar,b=5,p=0.01 iact-$TYP $FLAGS
./run_aov.sh normalization arbitrarge $INPUT $SEED reexec:t=scar,b=5,p=0.01 iact-$TYP $FLAGS
./run_aov.sh normalization unknown $INPUT $SEED reexec:t=scar,b=5,p=0.01 iact-$TYP $FLAGS

./run_aov.sh batching batch1 $INPUT $SEED reexec:t=scar,b=5,p=0.01 iact-$TYP $FLAGS
./run_aov.sh batching batch2 $INPUT $SEED reexec:t=scar,b=5,p=0.01 iact-$TYP $FLAGS
./run_aov.sh batching batch4 $INPUT $SEED reexec:t=scar,b=5,p=0.01 iact-$TYP $FLAGS
./run_aov.sh batching batch8 unknown $INPUT $SEED reexec:t=scar,b=5,p=0.01 iact-$TYP $FLAGS

./run_aov.sh selection out $INPUT $SEED reexec:t=scar,b=5,p=0.01 iact-$TYP $FLAGS
./run_aov.sh selection inout $INPUT $SEED reexec:t=scar,b=5,p=0.01 iact-$TYP $FLAGS
./run_aov.sh selection all $INPUT $SEED reexec:t=scar,b=5,p=0.01 iact-$TYP $FLAGS
