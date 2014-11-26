#!/bin/bash

mv -f TIMES RESULTS 2>/dev/null

for i in `seq 1 200` ; do
    ./water 4 | awk ' $0 ~ / 12 / { print $0 >> "RESULTS"; } $0 ~ /execution time/ { print $4 >> "TIMES"; }'; 
done

cat TIMES | awk ' {sum += $0} END { print "Average time = ", sum/NR }'