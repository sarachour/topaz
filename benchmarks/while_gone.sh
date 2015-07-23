#!/bin/bash

CURRDIR=$(pwd)

cd barnes; ./run_batching.sh batch4
cd $CURRDIR
cd water; ./run_batching.sh batch5
cd $CURRDIR
cd bodytrack; ./run_batching.sh nobatch
cd $CURRDIR
cd blackscholes; ./run_batching.sh batch64
cd $CURRDIR
cd streamcluster; ./run_stream_batch.sh
cd $CURRDIR
