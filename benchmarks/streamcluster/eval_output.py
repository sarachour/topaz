#!/usr/bin/python

import sys
import math
import numpy as np

name1 = sys.argv[1]
name2 = sys.argv[2]

if len(sys.argv) < 3:
	print("USAGE: pts clust")
	sys.exit(1)
fpts = open(name1, 'r');
fclust = open(name2, 'r');
# find closest cluster for each point.

def calc_clust_dist(pts,assigns,i,j):
	dist=0;
	for idx1 in assigns[i]:
		x1 = pts[idx1][0];
		y1 = pts[idx1][1];
	for idx2 in assigns[j]:
		x2 = pts[idx2][0];
		y2 = pts[idx2][1];
		ex = abs(x1-x2);
		ey = abs(y1-y2);
		err = ex*ex+ey*ey;
		dist += math.sqrt(err);
	dist /= len(assigns[j])*len(assigns[i]);
	return dist;
	
pts = [];
for pt in fpts:
	fcoords=pt.split('\t')
	x = float(fcoords[0]);
	y = float(fcoords[1]);
	pts.append([x,y]);

clusts = [];
assigns = [];
for line in fclust:
	elems=line.split(' ')
	if(len(elems) == 3):
		x = float(elems[0]);
		y = float(elems[1]);
		clusts.append([x,y]);
		assigns.append([]);
		
j=0;
# find assignments
for pt in pts:
	px = pt[0];
	py = pt[1];
	err = 0;
	grp = -1;
	i=0;
	for clust in clusts:
		ex = abs(clust[0]-px);
		ey = abs(clust[1]-py);
		terr = ex*ex+ey*ey;
		if(grp < 0 or terr < err):
			grp = i;
			err = terr;
		i+=1;
	assigns[grp].append(j);
	j+=1;

print "Cluster\tx\ty\tSilhouette"
stats = {'max':-1, 'min':-1,'avg':[]};
for i in range(0, len(clusts)):
	cerr=calc_clust_dist(pts,assigns,i,i)
	intererr = -1;
	for j in range(0,len(clusts)):
		if(i != j):
			ierr = calc_clust_dist(pts,assigns,i,j);
			if intererr < 0 or ierr < intererr:
				intererr = ierr; 
				
	if len(clusts) == 1:
		silhouette = cerr
	else:
		silhouette = (intererr - cerr)/max(intererr,cerr);
	if(stats['max'] < 0):
		stats['max'] = silhouette;
		stats['min'] = silhouette;
	
	stats['max'] = max(stats['max'], silhouette);
	stats['min'] = min(stats['min'], silhouette);
	stats['avg'].append(silhouette);
	print i," \t",clusts[i][0],"\t",clusts[i][1],"\t",silhouette

print "-----------------------"
print "Best CLuster:", stats['max'];
print "Worst Cluster:", stats['min'];
print "Average Score:", np.mean(stats['avg']);
print "Median Score:", np.median(stats['avg']);
