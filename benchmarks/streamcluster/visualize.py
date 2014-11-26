#!/usr/bin/python

import matplotlib.pyplot as plt;
import sys;

if len(sys.argv) < 2:
	print "USAGE: ./visualize.py points [clusters]"
	sys.exit(1)
	

clustx = [];
clusty = [];
if(len(sys.argv) >= 3):
	clust_name = sys.argv[2];
	f = open(clust_name, 'r');
	for line in f:
		elems=line.split(' ');
		if(len(elems) >= 2):
			clustx.append(float(elems[0]));
			clusty.append(float(elems[1]));
	
pt_name = sys.argv[1];
f = open(pt_name, 'r');
		
x = [];
y = [];
for line in f:
	coords = line.split('\t');
	x.append(float(coords[0]));
	y.append(float(coords[1]));

fig = plt.figure();
plt.scatter(x,y,c='k', marker='o',s=1);
plt.scatter(clustx,clusty,c='r', marker='o',s=200);

plt.savefig('diagram.png');
