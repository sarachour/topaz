#!/bin/bash

NRANDS=$1

for i in $(seq $NRANDS);
do
	echo $RANDOM 
done
