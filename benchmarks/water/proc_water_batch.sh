#!/bin/bash
#!/bin/bash

proc_generic.sh output-batch8 'grep -E "Average Vector Pos Pct Err:[ 0-9\.e\-]+$" | grep -o -E "[0-9\.e\-]+$"'

gen_generic_graph.py summary Water "Percent Position Error" 3
