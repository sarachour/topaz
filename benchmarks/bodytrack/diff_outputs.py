#!/usr/bin/python

import sys
import math
print "RUNNING"
name1 = sys.argv[1]
name2 = sys.argv[2]
print name1, name2
file1 = open(name1, 'r');
file2 = open(name2, 'r');

vect1=[];
vect2=[];
res=[];
for (l1,l2) in zip(file1,file2):
	coords1=l1.split(' ');
	coords2=l2.split(' ');
	diff=[]
	svect1=[];
	svect2=[];
	for (c1,c2) in zip(coords1,coords2):
		print c1,c2
		try:
			v1 = float(c1);
			v2 = float(c2);
			err = abs(v1-v2);
			svect1.append(v1);
			svect2.append(v2);
			diff.append(err)
		except ValueError:
			svect1.append(1);
			svect2.append(1);
			diff.append(0);
	res.append(diff)
	vect1.append(svect1);
	vect2.append(svect2);

i=0;
terr = 0;
tfracerr = 0;
for (fres,fvect1,fvect2) in zip(res,vect1,vect2):
	fracerr=0;
	err=0;
	weights=[];
	scales=[];
	sum=0;
	for i in range(0,len(fres)):
		if(i == 3 or i==4 or i==5):
			weight = 10;
			scale = fvect1[i];
		else:
			weight = 1;
			scale = math.pi;
		sum += weight;
		weights.append(weight);
		scales.append(scale);
		
	for (s,w,cr,c1,c2) in zip(scales,weights,fres,fvect1,fvect2):
		err += cr*cr*w/sum;
		fracerr += cr*cr/(s*s)*w/sum
		
	err=math.sqrt(err);
	fracerr=math.sqrt(fracerr)*100.0;
	print "FRAME ",str(i),": error=",err,", pct-err=",fracerr
	i +=1
	tfracerr += fracerr;
	terr += terr;

tfracerr/=len(res);
terr/=len(res);
print "================"
print "OVER ",str(i)," FRAMES: avg-error=",terr,", avg-pct-err=",tfracerr
file1.close();
file2.close();	
