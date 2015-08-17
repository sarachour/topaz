
TAG=$1

rm water.tpz
cp "water.$TAG.tpz" water.tpz 
make clean; make || exit 1
rm -rf output
cp -r "output-$TAG" output
if [ ! -d output ]; then
	echo "no output directory"
	exit 1
fi
./run_water_batch.sh
rm -rf output-$TAG
mv output output-$TAG
