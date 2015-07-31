#!/bin/bash

proc_generic.sh output 'grep -E "Average Pct Vector Err:[ 0-9\.e\-]+$" | grep -o -E "[0-9\.e\-]+$"'

gen_generic_graph.py summary Barnes "Percent Position Error" 2
