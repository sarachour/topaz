#!/bin/bash
# change LDPATH in configure.ac to add dependencies.

cd src;
export VPATH=$(pwd)/TrackingBenchmark;
#make clean;
#autoconf;
#./configure VPATH=$(pwd);
cd TrackingBenchmark;
tpzcc main.tpz &&
mv main.topaz.c main.cpp &&
cd .. &&
./configure VPATH=$(pwd) &&
make 
