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
BADEPS = 100
total=0;
nerrs = 0;
nbaderrs = 0
skips = 0
cerr = [0,0,0];
cvec = [0,0,0];

for (line1,line2) in zip(str1.splitlines(), str2.splitlines()):
	fields1 = line1.split('\t');
	fields2 = line2.split('\t');
	
	if(len(fields1) == 10 and len(fields2) == 10):
		total += 3
		try:
			h1_1=[float(fields1[1]), float(fields1[2]), float(fields1[3])];
			h1_2=[float(fields2[1]), float(fields2[2]), float(fields2[3])];

			o_1=[float(fields1[4]), float(fields1[5]), float(fields1[6])];
			o_2=[float(fields2[4]), float(fields2[5]), float(fields2[6])];

			h2_1=[float(fields1[7]), float(fields1[8]), float(fields1[9])];
			h2_2=[float(fields2[7]), float(fields2[8]), float(fields2[9])];

			if(math.isnan(h1_2[0]) or math.isnan(h1_2[1]) or math.isnan(h1_2[2])):
				raise ValueError 

			if(math.isnan(o_2[0]) or math.isnan(o_2[1]) or math.isnan(o_2[2])):
				raise ValueError 

			if(math.isnan(h2_2[0]) or math.isnan(h2_2[1]) or math.isnan(h2_2[2])):
				raise ValueError 


			cvec[0] += h1_1[0]; cvec[1] += h1_1[1]; cvec[2] += h1_1[2]
			cvec[0] += o_1[0]; cvec[1] += o_1[1]; cvec[2] += o_1[2]
			cvec[0] += h2_1[0]; cvec[1] += h2_1[1]; cvec[2] += h2_1[2]
			
			err = math.sqrt(pow(h1_1[0] - h1_2[0],2) + pow(h1_1[1] - h1_2[1],2) + pow(h1_1[2] - h1_2[2],2))
			 
			if (err > EPS):
				nerrs += 1
				cerr[0] += abs(h1_1[0] - h1_2[0])
				cerr[1] += abs(h1_1[1] - h1_2[1])
				cerr[2] += abs(h1_1[2] - h1_2[2])
			
			if(err > BADEPS):
				nbaderrs +=1
				
			err = math.sqrt(pow(o_1[0] - o_2[0],2) + pow(o_1[1] - o_2[1],2) + pow(o_1[2] - o_2[2],2))
			 
			if (err > EPS):
				nerrs += 1
				cerr[0] += abs(o_1[0] - o_2[0])
				cerr[1] += abs(o_1[1] - o_2[1])
				cerr[2] += abs(o_1[2] - o_2[2])
				
			if(err > BADEPS):
				nbaderrs +=1
				
			err = math.sqrt(pow(h2_1[0] - h2_2[0],2) + pow(h2_1[1] - h2_2[1],2) + pow(h2_1[2] - h2_2[2],2))
			 
			if (err > EPS):
				nerrs += 1
				cerr[0] += abs(h2_1[0] - h2_2[0])
				cerr[1] += abs(h2_1[1] - h2_2[1])
				cerr[2] += abs(h2_1[2] - h2_2[2])
				
			if(err > BADEPS):
				nbaderrs +=1	
				
		except ValueError:
			skips+=1;
		except OverflowError:
			skips+=1;
			
print "Total: ", total
print "Skipped: ", skips
print "Number Errors: ", nerrs
print "Number Really Bad Errors: ", nbaderrs
print "Total Errorneous Pos Err: ", cerr
print "Average Vector Pos: ", [cvec[0]/total, cvec[1]/total,cvec[2]/total]
print "Average Vector Pos Err: ", [cerr[0]/nerrs, cerr[1]/nerrs,cerr[2]/nerrs]
print "Average Vector Pos Err: ", math.sqrt(pow(cerr[0]/nerrs,2) + pow(cerr[1]/nerrs,2)+pow((cerr[2]/nerrs),2))


print "Average Vector Pos Pct Err: ", [(cerr[0]/nerrs) / (cvec[0]/total)*100, (cerr[1]/nerrs) /(cvec[1]/total)*100,(cerr[2]/nerrs)/(cvec[2]/total)*100]
print "Average Vector Pos Pct Err: ", math.sqrt(pow((cerr[0]/nerrs) / (cvec[0]/total),2) + pow((cerr[1]/nerrs) /(cvec[1]/total),2)+pow((cerr[2]/nerrs)/(cvec[2]/total),2))*100
