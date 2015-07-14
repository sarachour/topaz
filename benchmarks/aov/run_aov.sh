
CATEGORY=$1
KIND=$2


INPUT=$3
SEED=$4
TOPAZ_ARGS=$5
PIN_ARGS=$6
TAGS=$7


ODIR=output/$CATEGORY/$KIND
SRCDIR=kernels/$CATEGORY/$KIND
CDIR=$PWD

if [ ! -f output/correct ]; 
then
	mkdir -p output/correct
fi

if [ "$CATEGORY" = "correct" ];
then
	rm -rf src/output/*
	cp -r output/correct/* src/output/
	cd src; make clean; make && sutil_run_script.sh run_bs.sh $INPUT $SEED none perfect &> /dev/null
	cd $CDIR
	cp -r src/output/* output/correct/
	exit 0;
fi

if [ ! -f $ODIR/ ]; 
then
	mkdir -p $ODIR/
fi
if [ ! -f src/output ]; 
then
	mkdir -p src/output
fi



#sutil_run_script.sh run_bs.sh 16K 1 reexec:t=scar,b=5,p=0.10 heavy-dram d
cp $SRCDIR/blackscholes.tpz src/blackscholes.tpz
rm -rf src/output/*
cp -r $ODIR/* src/output/
cp -r output/correct/* src/output/

echo sutil_run_script.sh run_bs.sh $INPUT $SEED $TOPAZ_ARGS $PIN_ARGS $TAGS;

cd src; make clean; make &&
	sutil_run_script.sh run_bs.sh $INPUT $SEED $TOPAZ_ARGS $PIN_ARGS $TAGS &> /dev/null;
cd $CDIR
cp -r src/output/* $ODIR/
