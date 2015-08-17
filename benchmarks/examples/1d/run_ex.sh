make clean; make

PIN_ARGS=$1
TOPAZ_ARGS=$2
TAGS=$3
INPUT=$4
SEED=$5

source sutil_make_dirs.sh


function createDirectories {
	baseCreateDirectories $1
}

function updateDirectories {
	outdir=$1
	suff=$2
	baseUpdateDirectories $outdir $suff
	mv out.txt output/$outdir/out.$suff.txt
	mv err.txt output/$outdir/err.$suff.txt
	mv log.txt output/$outdir/log.$suff.txt
	mv figure.png output/$outdir/fig.$suff.png
}

OUTDIR=$TAGS
SUFFIX="$INPUT.$SEED"
#sutil_run_script.sh run_ex.sh d1.n100.k1 1 reexec:t=scar,b=5,p=0.10 iact-med-static d
#sutil_run_script.sh run_ex.sh d1.n100.k1 1 none iact-med-static

ND=$(echo "$INPUT" | grep -o "d[0-9]*" | grep -o "[0-9]*" )
NPTS=$(echo "$INPUT" | grep -o "n[0-9]*"| grep -o "[0-9]*")
NITERS=$(echo "$INPUT" | grep -o "k[0-9]*"| grep -o "[0-9]*")

createDirectories $OUTDIR $SUFFIX
tpzrun $PIN_ARGS -- ./ex1d $TOPAZ_ARGS @ $ND $NPTS out.txt $NITERS  > log.txt
./disp_out.py out.txt
updateDirectories $OUTDIR $SUFFIX
