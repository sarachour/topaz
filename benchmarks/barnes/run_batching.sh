
TAG=$1

rm src/nbody_sim.tpz
cp "src/nbody_sim.$TAG.tpz" src/nbody_sim.tpz 
make clean; make || exit 1
rm -rf output
cp -r "output-$TAG" output
if [ ! -d output ]; then
	echo "no output directory"
	exit 1
fi
./run_barn_batch.sh
rm -rf output-$TAG
mv output output-$TAG
