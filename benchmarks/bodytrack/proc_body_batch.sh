#!/bin/bash

proc_generic.sh output 'grep -E "avg-pct-err=[ 0-9\.]+$" | grep -o -E "[0-9\.]+$"'

gen_generic_graph.py summary BodyTrack "Percent Pose Error" 2

