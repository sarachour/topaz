#!/bin/bash

proc_generic.sh output-batch64 'grep -E "Percent Price:[ 0-9\.\-e]+$" | grep -o -E "[0-9\.\-e]+$"'

#gen_generic_graph.py summary BlackScholes "Percent Price Error"
