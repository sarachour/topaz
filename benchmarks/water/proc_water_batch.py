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
				
		#stdev = scipy.std(data)
		avg = numpy.percentile(data, 50)
		plot[typ][bs][prob]["median"] =avg 
		plot[typ][bs][prob]["low"] = avg-numpy.percentile(data, 25)
		plot[typ][bs][prob]["high"] = numpy.percentile(data, 75)-avg
		
		
	except ValueError:
		print "skipped ",line


def q_bs(v):
	return plot["normal"][v][0.01] 
	
def q_pr(v):
	return  plot["normal"][5][v]



plt.figure()
plt.title("Block Size vs Percent Error")
indep = [1,2,3,4]
data= map(lambda x : q_bs(x+1),indep)
avg = map(lambda x : x["median"],data) 
lw = map(lambda x : x["low"],data) 
hi = map(lambda x : x["high"],data) 
plt.errorbar(indep, avg, yerr=[lw,hi], fmt='o--')
plt.savefig("bs.png")

plt.figure()
plt.title("Target Probability vs Percent Error")
indep = [0,0.01,0.02,0.04]
data= map(lambda x : q_pr(x),indep)
avg = map(lambda x : x["median"],data) 
lw = map(lambda x : x["low"],data) 
hi = map(lambda x : x["high"],data) 
plt.errorbar(indep, avg, yerr=[lw,hi], fmt='o--')
plt.savefig("targ-prob.png")
