
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
	baseUpdateDirectories $1 $2
	outdir=$1
	suff=$2
	mv price.txt output/$outdir/price.$suff.txt
	mv err.txt output/$outdir/err.$suff.txt
	mv log.txt output/$outdir/log.$suff.txt
}

OUTDIR=$TAGS
SUFFIX="$INPUT.$SEED"

#sutil_run_script.sh run_ex.sh 1.100 1 reexec:t=scar,b=5,p=0.10 heavy-dram d
#sutil_run_script.sh run_ex.sh 1.100 1 basic heavy-dram 

ND=$(echo "$INPUT" | grep -o "^[0-9]*")
NPTS=$(echo "$INPUT" | grep -o "[0-9]*$")

echo $ND $NPTS
createDirectories $OUTDIR
tpzrun $PIN_ARGS -- ./ex1d $TOPAZ_ARGS @ $ND $NPTS out.txt  > log.txt
./diff_output.py output/perfect.none*/price.*.txt price.txt > err.txt
updateDirectories $OUTDIR $SUFFIX
