#!/bin/bash

mv -f TIMES RESULTS 2>/dev/null
NPROC=4

for i in `seq 1 20` ; do
#    ./water-S $NPROC | awk ' $0 ~ / 150 / { print $0 >> "RESULTS-SEQ-4"; } $0 ~ /execution time/ { print $4 >> "TIMES-SEQ"; }'; 
    ./water-PR 4 | awk ' $0 ~ / 150 / { print $0 >> "RESULTS-PR-4"; } $0 ~ /execution time/ { print $4 >> "TIMES-PR-4"; }'; 
    ./water-PIPR 4 | awk ' $0 ~ / 150 / { print $0 >> "RESULTS-PIPR-4"; } $0 ~ /execution time/ { print $4 >> "TIMES-PIPR-4"; }'; 
done

cat TIMES-SEQ-$NPROC | awk ' {sum += $0} END { print "Average time SEQ = ", sum/NR }'
cat TIMES-PR-$NPROC | awk ' {sum += $0} END { print "Average time PR = ", sum/NR }'
cat TIMES-PIPR-$NPROC | awk ' {sum += $0} END { print "Average time PIPR = ", sum/NR }'