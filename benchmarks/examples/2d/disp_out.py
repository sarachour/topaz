#!/usr/bin/python
import sys;
import matplotlib.pyplot as plt
import random
if len(sys.argv) > 1:
	filename=sys.argv[1];
else:
	print "USAGE: disp_out.py file"
	exit(1)
	
print filename

file = open(filename)
dx = [];
dy = [];
for line in file:
	field = line.split('\t');
	nx=float(field[0]);
	ny=float(field[1]);
	dx.append(nx)
	dy.append(ny)

plt.plot(dx, dy, 'ko');

plt.savefig("figure.png");
