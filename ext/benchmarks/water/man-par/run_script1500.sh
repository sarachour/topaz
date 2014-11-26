#!/bin/bash

mv -f TIMES RESULTS 2>/dev/null
NPROC=8

for i in `seq 1 10` ; do
#    ./water-S $NPROC | awk ' $0 ~ / 150 / { print $0 >> "RESULTS-SEQ-8"; } $0 ~ /execution time/ { print $4 >> "TIMES-SEQ"; }'; 
    ./water-PR 8 | awk ' $0 ~ / 150 / { print $0 >> "RESULTS-PR-8"; } $0 ~ /execution time/ { print $4 >> "TIMES-PR-8"; }'; 
    ./water-PIPR 8 | awk ' $0 ~ / 150 / { print $0 >> "RESULTS-PIPR-8"; } $0 ~ /execution time/ { print $4 >> "TIMES-PIPR-8"; }'; 
done

cat TIMES-SEQ-$NPROC | awk ' {sum += $0} END { print "Average time SEQ = ", sum/NR }'
cat TIMES-PR-$NPROC | awk ' {sum += $0} END { print "Average time PR = ", sum/NR }'
cat TIMES-PIPR-$NPROC | awk ' {sum += $0} END { print "Average time PIPR = ", sum/NR }'