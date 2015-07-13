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
			try:
				data.append(float(fields[i]))
			except ValueError:
				print "continuing."
			
		print len(fields) - 3 
		if not (typ in plot):
			plot[typ]={}
		
		if not (bs in plot[typ]):
			plot[typ][bs]={}
		
		if not (prob in plot[typ][bs]):
			plot[typ][bs][prob]={}
				
		avg = scipy.mean(data)
		stdev = scipy.std(data)
		plot[typ][bs][prob]["avg"] = (avg)
		plot[typ][bs][prob]["stdev"] = (stdev)
		
		
	except ValueError:
		print "skipped ",line


def q_bs(v):
	res={}
	print v,plot["normal"][v][0.01]
	res["avg"] = plot["normal"][v][0.01]["avg"] 
	res["std"] = plot["normal"][v][0.01]["stdev"] 
	return res 
	
def q_pr(v):
	res={}
	res["avg"] = plot["normal"][5][v]["avg"] 
	res["std"] = plot["normal"][5][v]["stdev"] 
	return res



plt.figure()
plt.title("Block Size vs Percent Error")
indep = [1,2,3,4]
data= map(lambda x : q_bs(x+1),indep)
avg = map(lambda x : x["avg"],data) 
std = map(lambda x : x["std"],data) 
plt.errorbar(indep, avg, yerr=std, fmt='o--')
plt.savefig("bs.png")

plt.figure()
plt.title("Target Probability vs Percent Error")
indep = [0,0.01,0.02,0.04]
data= map(lambda x : q_pr(x),indep)
avg = map(lambda x : x["avg"],data) 
std = map(lambda x : x["std"],data) 
plt.errorbar(indep, avg, yerr=std, fmt='o--')
plt.savefig("targ-prob.png")
