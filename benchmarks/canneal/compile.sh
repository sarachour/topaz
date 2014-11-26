#!/bin/bash

# Main kernel: loop surrounding calculate_delta_routing_cost
cd src;
make clean;
autoconf;
./configure;
tpzcc -c main.tpz;
mv main.topaz.c main.cpp
make;
