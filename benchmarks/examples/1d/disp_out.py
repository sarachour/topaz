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
data = [];

for line in file:
	num=float(line)
	data.append(num)

xmin = min(data)
xmax = max(data)
y=0
ht=0.5

plt.hlines(y, xmin, xmax);
plt.vlines(xmin,y-ht/2., y + ht/2.)
plt.vlines(xmax,y-ht/2., y + ht/2.)

for e in data:
	plt.plot(e,random.uniform(y-ht/2,y+ht/2.),'ko', ms=3, mfc='k')

plt.text(xmin - 5, y, int(xmin), horizontalalignment="right")
plt.text(xmax + 5, y, int(xmax), horizontalalignment="left")
plt.axis('off')
plt.savefig("figure.png");
