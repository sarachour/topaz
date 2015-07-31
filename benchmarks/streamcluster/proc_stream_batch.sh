#!/bin/bash

proc_generic.sh output 'grep -E "Average Score:[ 0-9\.e\-]+$" | grep -o -E "[0-9\.e\-]+$"'

gen_generic_graph.py summary Streamcluster "Center Score" 2

