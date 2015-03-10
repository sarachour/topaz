
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
#sutil_run_script.sh run_ex.sh 1.100 1 reexec:t=scar,b=5,p=0.10 heavy-dram d
#sutil_run_script.sh run_ex.sh 1.100 1 none heavy-dram 
#sutil_run_script.sh run_ex.sh 1.100 1 reexec:t=dist,b=1,p=0.10 heavy-dram d

ND=$(echo "$INPUT" | grep -o "^[0-9]*")
NPTS=$(echo "$INPUT" | grep -o "[0-9]*$")

createDirectories $OUTDIR $SUFFIX
tpzrun $PIN_ARGS -- ./ex1d $TOPAZ_ARGS @ $ND $NPTS out.txt  > log.txt
./disp_out.py out.txt
updateDirectories $OUTDIR $SUFFIX
