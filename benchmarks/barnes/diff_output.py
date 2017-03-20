#!/usr/bin/python

import sys
import math

name1 = sys.argv[1]
name2 = sys.argv[2]

file1 = open(name1, 'r');
str1=file1.read();
file1.close()

file2 = open(name2, 'r');
str2=file2.read()
file2.close();

EPS=1e-6

total=0;
nerrs = 0;
cerr = [0,0,0];
cvec = [0,0,0];

for (line1,line2) in zip(str1.splitlines(), str2.splitlines()):
	fields1 = line1.split(',');
	fields2 = line2.split(',');
	
	if(len(fields1) == 5 and len(fields2) == 5):
		total += 1
		try:
			p1=[float(fields1[2]), float(fields1[3]), float(fields1[4])];
			p2=[float(fields2[2]), float(fields2[3]), float(fields2[4])];
			
			err = math.sqrt( pow(p1[0]-p2[0],2) + pow(p1[1]-p2[1],2) + pow(p1[2]-p2[2],2));

			cvec[0] += abs(p1[0])
			cvec[1] += abs(p1[1])
			cvec[2] += abs(p1[2])
		
			if (err > EPS):
				nerrs += 1
				cerr[0] += abs(p1[0] - p2[0]);
				cerr[1] += abs(p1[1] - p2[1]);
				cerr[2] += abs(p1[2] - p2[2]);
			
		except ValueError:
			print "cannot parse"
		except OverflowError:
			print "overflow."

apos = [cvec[0]/total, cvec[1]/total,cvec[2]/total];
aerr = [cerr[0]/nerrs, cerr[1]/nerrs,cerr[2]/nerrs]
apct = [aerr[0]/apos[0]*100, aerr[1]/apos[1]*100, aerr[2]/apos[2]*100]
print "== Positional Statistics (AbsVal Pos) =="
print "Total: ", total
print "Number Errors: ", nerrs
print "Cuml Pos: ", cvec
print "Cuml Err: ", cerr
print "Average Vector Pos: ", apos
print "Average Vector Err: ", aerr
print "Average Vector Err: ", math.sqrt(pow(aerr[0],2)+pow(aerr[1],2)+pow(aerr[2],2))
print "Average Pct Vector Err: ", apct
print "Average Pct Vector Err: ", math.sqrt(pow(apct[0],2)+pow(apct[1],2)+pow(apct[2],2))
