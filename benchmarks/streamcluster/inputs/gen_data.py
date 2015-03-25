#!/usr/bin/python

from random import *
import sys

if len(sys.argv) < 3:
	print "USAGE: ./gen_data.py ncluster pts blksize file"
	sys.exit(1)
	
ncluster = int(sys.argv[1]);
npts = int(sys.argv[2]);
BLK_SIZE=int(sys.argv[3]);
file = sys.argv[4];

centers=[];


for i in range(0,ncluster):
  node = {};
  node['avg.x'] = random()
  node['avg.y'] = random()
  node['std.x'] = node['std.y']= random()*0.10+0.02;
  print node
  centers.append(node);
  
pts = [];
nblks=(npts+BLK_SIZE-1)/BLK_SIZE;
blkspercenter=int(nblks/ncluster);
for i in range(0,nblks):
	if(random() > 0.25):
		avg_x=0.5;
		avg_y=0.5;
		std_x=0.5;
		std_y=0.5;
	else:
		node = centers[(i%blkspercenter)%ncluster]
		avg_x=node['avg.x']
		avg_y=node['avg.y']
		std_x=node['std.x']
		std_y=node['std.y']
	
	siz = npts - BLK_SIZE*i;
	if(siz > BLK_SIZE):
		siz = BLK_SIZE 
		
	for j in range(0,siz):
	  x = gauss(avg_x, std_x);
	  y = gauss(avg_y, std_y);
	  if(x < 0):
		x = 0;
	  if(x > 1):
		x = 1;
	  if(y < 0):
		y = 0;
	  if(y > 1):
		y = 1;
	  pts.append((x,y));
		


f = open(file, 'w');
for i in range(0,npts):
  pt = pts[i];
  line = str(pt[0]) + "\t"+str(pt[1])+"\n";
  f.write(line)
