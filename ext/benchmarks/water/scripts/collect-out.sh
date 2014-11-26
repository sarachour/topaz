
SEQ_T=5.06

if [ "x$1" == "x" ]; then
    echo "no destination directory selected. Aborting..."
    exit 1
fi

mkdir -p $1
rm -rf $1/*
mv PAR* $1
cd $1
for p in `find -name "PAR-TIME*"`; do cat $p | awk  ' {if (/Wall/) print 5.06/$3} ' ; done > SPDS 
for p in `find -name "PAR-OUT*"`; do  distl.py ../SEQ-OUT.1000 $p | awk '{if (/Max/) print $3}'; done > DISTS
paste -d ", " SPDS DISTS | sed 's/,/, /g'

cd -