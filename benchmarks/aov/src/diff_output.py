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

nerrs = 0;
cerr = 0;
total=0;
cprice=0;
cerrprice=0
for (line1,line2) in zip(str1.splitlines(), str2.splitlines()):
	fields1 = line1.split('\t');
	fields2 = line2.split('\t');
	
	if(len(fields1) == 1 and len(fields2) == 1):
		total += 1
		p1=float(fields1[0]);
		p2=float(fields2[0]);
		err = abs(p1-p2);
		cprice += p1;
		if (err > EPS):
			nerrs += 1
			cerr += err
			cerrprice += p1

print "Total: ", total
print "Number Errors: ", nerrs
print "Total Errorneous Price Err: ", cerr
print "Total Errorneous Price: ", cerrprice
print "Total Price: ", cprice
print "Percent Price: ", (cerr/cprice*100)
print "-------"
print "Percent Errors:", (float(nerrs)/total*100)
print "Average Price Error:", (cerr/nerrs)
print "Average Price:", ((cprice)/total)
