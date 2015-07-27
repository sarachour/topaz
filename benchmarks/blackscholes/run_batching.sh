
TAG=$1

rm blackscholes.tpz
cp "blackscholes.$TAG.tpz" blackscholes.tpz 
make clean; make || exit 1
rm -rf output
cp -r "output-$TAG" output
if [ ! -d output ]; then
	echo "no output directory"
	exit 1
fi
./run_bs_batch.sh
rm -rf output-$TAG
mv output output-$TAG
