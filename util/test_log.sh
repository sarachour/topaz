#!/bin/bash

LOGFILE=$1

TEST1=`cat $LOGFILE | grep -F "++ Main...Continuing" | wc -l`
TEST2=`cat $LOGFILE | grep -F "[WORKER] Triggering Finalize..." | wc -l`
TEST3=`cat $LOGFILE | grep -F "++ Worker...Finalizing" | wc -l`
TEST4=`cat $LOGFILE | grep -F "++ Worker...Exiting" | wc -l`

SUM=`echo $TEST1 + $TEST2 + $TEST3 + $TEST4 | bc -l`

if [ "$SUM" -eq "4" ]
then
   echo "Passed.. Log is complete"
else
   echo "Failed.. Manually view log to see if the program exited prematurely"
fi
