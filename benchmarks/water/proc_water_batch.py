#!/usr/bin/python
import matplotlib.pyplot as plt
import numpy
import scipy

filename="summary.txt"
file=open(filename,"r")

plot={}
for line in file:
	try:
		fields=line.split(",");
		
		typ=fields[2]
		bs=int(fields[1])
		prob=float(fields[0])
		data=[]
		
		for i in range(3,len(fields)):
			data.append(float(fields[i]))
		
		if not (typ in plot):
			plot[typ]={}
			plot[typ]["bs"]=[]
			plot[typ]["targ-prob"]=[]
			plot[typ]["avg"]=[]
			plot[typ]["stdev"]=[]
		
		elem={}
		plot[typ]["bs"].append(bs)
		plot[typ]["targ-prob"].append(prob)
		
		avg = scipy.mean(data)
		stdev = scipy.std(data)
		plot[typ]["avg"].append(avg)
		plot[typ]["stdev"].append(stdev)
		
		
	except ValueError:
		print "skipped"

print plot

plt.figure()
plt.title("Block Size vs Percent Error")
plt.errorbar(plot['normal']['bs'], plot['normal']['avg'], yerr=plot['normal']['stdev'], fmt='o--')
plt.savefig("bs.png")

plt.figure()
plt.title("Target Probability vs Percent Error")
plt.errorbar(plot['normal']['targ-prob'], plot['normal']['avg'], yerr=plot['normal']['stdev'], fmt='o--')
plt.savefig("targ-prob.png")
