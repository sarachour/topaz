#!/bin/bash
CHILD_SCRIPT=$1
INPUT=$2
NRANDS=$3
OP=$4
MACHINE=$5
FLAGS=$6

echo "USAGE: CHILD_SCRIPT INPUT NRANDS [reexec,discard,none]:p=#.##,b=###,t=[scar,solid,autoscar] MACHINE 'FLAGS[g=--godmode t=--time d=--log-detector]' "
TOPAZ_ARGS=""
TAGS=""
PIN_ARGS=" -e -f $MACHINE.cfg "

declare -A typemap

typemap['scar']='s';
typemap['solid']='x';
typemap['dist']='d';
typemap['autoscar']='c';

if [[ $FLAGS == *g* ]]
then
	TOPAZ_ARGS=$TOPAZ_ARGS" --godmode"
	TAGS=$TAGS".god"
fi
if [[ $FLAGS == *t* ]]
then
	TOPAZ_ARGS=$TOPAZ_ARGS" --time"
	TAGS=$TAGS".ltime"
fi


if [[ $FLAGS == *d* ]]
then
	TOPAZ_ARGS=$TOPAZ_ARGS" --log-detector"
	TAGS=$TAGS".ldet"
fi

if [[ $OP == *none* ]]
then
	
	TAGS="$MACHINE.none$TAGS"
fi

if [[ $OP == *reexec* ]]
then
	AMOUNT=$(echo "$OP" | grep -o "p=[0-9\.]*" | grep -o "[0-9\.]*")
	BLKS=$(echo "$OP" | grep -o "b=[0-9]*" | grep -o "[0-9\.]*")
	TYPE=$(echo "$OP" | grep -o "t=[a-z]*" | grep -o "[a-z]*$")
	TOPAZ_ARGS=$TOPAZ_ARGS" --detect "${typemap[$TYPE]}" --detect-target "$AMOUNT" --detect-blocks "$BLKS
	TAGS="$MACHINE.reexec.$TYPE.p"$AMOUNT".b"$BLKS"$TAGS"
fi

if [[ $OP == *discard* ]]
then
	AMOUNT=$(echo "$OP" | grep -o "p[0-9\.]*" | grep -o "[0-9\.]*")
	BLKS=$(echo "$OP" | grep -o "b[0-9]*" | grep -o "[0-9\.]*")
	TYPE=$(echo "$OP" | grep -o "t=[a-z]*" | grep -o "[a-z]*")
	TOPAZ_ARGS=$TOPAZ_ARGS" --discard --detect "${typemap[$TYPE]}" --detect-target "$AMOUNT" --detect-blocks "$BLKS
	TAGS="$MACHINE.discard.$TYPE.p"$AMOUNT".b"$BLKS"$TAGS"
fi

if [[ "$TAGS" -eq "" ]]; then
	TAGS="none"
fi 

for num in $(head $TOPAZ_ROOT/util/rands.txt -n $NRANDS);
do
	echo "PIN-ARGS : $PIN_ARGS"
	echo "TOPAZ-ARGS : $TOPAZ_ARGS"
	echo "TAGS : $TAGS"
	echo "INPUT : $INPUT"
	echo "SEED : $num"
	echo "==== Starting in 5 Seconds ==="
	sleep 3
	echo "==== Starting in 2 Seconds ==="
	sleep 2
	echo "==== Starting in 1 Second ==="
	sleep 2
	echo "#### Starting Run ####"

 ./$CHILD_SCRIPT "$PIN_ARGS" "$TOPAZ_ARGS" "$TAGS" $INPUT $num
done
mplayer $TOPAZ_ROOT/util/chime.mp3
