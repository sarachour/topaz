
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


if [ ! -d output/correct ]; 
then
	mkdir -p output/correct
fi

if [ -d src/output ]; 
then
	rm -rf src/output
fi

mkdir src/output

if [ "$CATEGORY" = "correct" ];
then
	echo "Running: CORRECT"
	cp kernels/correct/blackscholes.tpz src/blackscholes.tpz
	cd src; make clean &> /dev/null; 
		make &> /dev/null && sutil_run_script.sh run_bs.sh $INPUT $SEED none perfect &> /dev/null
	cd $CDIR
	cp -r src/output/* output/correct/
	exit 0;
fi

if [ ! -d $ODIR ]; 
then
	mkdir -p $ODIR
fi




echo "Copying: AOV $CATEGORY $KIND"
#sutil_run_script.sh run_bs.sh 16K 1 reexec:t=scar,b=5,p=0.10 heavy-dram d
cp $SRCDIR/blackscholes.tpz src/blackscholes.tpz
cp -r $ODIR/* src/output/
cp -r output/correct/* src/output/

 
echo "Running: AOV $CATEGORY $KIND"

cd src; 
make clean &> /dev/null || exit 1
make &> /dev/null || exit 1
sutil_run_script.sh run_bs.sh $INPUT $SEED $TOPAZ_ARGS $PIN_ARGS $TAGS &> /dev/null  || exit 1
cd $CDIR
cp -r src/output/* $ODIR/
