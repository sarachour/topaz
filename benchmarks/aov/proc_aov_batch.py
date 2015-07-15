#!/usr/bin/python
import matplotlib.pyplot as plt
import numpy
import scipy
import sys

filename="summary.txt"
file=open(filename,"r")

plot={}

def make_path(d,elems, stub):
	n = len(elems)
	q = d;
	for i in range(0,n-1):
		e = elems[i]
		if not (e in d):
			d[e] = {}
		d = d[e]
	
	d[elems[n-1]] = stub
	
	return q

def get_path(d,elems):
	n = len(elems)
	for i in range(0,n):
		e = elems[i]
		d = d[e]
	return d;
	
for line in file:
	try:
		fields=line.split(",");
		category=fields[0];
		kind=fields[1];
		prob=float(fields[2])
		bs=int(fields[3])
		typ=fields[4]
		data=[]
		
		print fields
		for i in range(5,len(fields)):
			try:
				data.append(float(fields[i]))
			except ValueError:
				print "continuing."
			
		
		path = [category,typ,kind]
		make_path(plot,path,{})
		
		#stdev = scipy.std(data)
		avg = numpy.percentile(data, 50)
		par = get_path(plot,path);
		par["median"] =avg 
		par["low"] = avg-numpy.percentile(data, 25)
		par["high"] = numpy.percentile(data, 75)-avg
		
		
	except ValueError:
		print "skipped ",line


def plot_info(title,indep_data,filename,conv):
	plt.figure()
	plt.margins(0.05, 0.05)
	plt.title(title)
	indep = indep_data
	indep_num = range(0,len(indep_data));
	data= map(lambda x : conv(x),indep)
	avg = map(lambda x : x["median"],data) 
	lw = map(lambda x : x["low"],data) 
	hi = map(lambda x : x["high"],data) 
	
	plt.errorbar(indep_num, avg, yerr=[lw,hi], fmt='o--')
	plt.xlabel('xlabel')
	plt.ylabel('ylabel')
	#plt.ylabel(indep)
	#plt.barplot(indep, avg)
	plt.savefig(filename)
	
print plot

try:
	title="Effect of Different Normalization Algorithms on Quality"
	conv = lambda x : get_path(plot,["normalization","normal",x])
	indep = ["naive","subtract","arbitrarge"]
	filename="norm.png"
	plot_info(title,indep,filename,conv);
except KeyError:
	print "Failed to produce aov normalization plot. continuing"

	
	
try:
	title="Effect of Different Batch Sizes on Quality"
	conv = lambda x : get_path(plot,["batching","normal",x])
	indep = ["batch1","batch2","batch4","batch8"]
	filename="batching.png"
	plot_info(title,indep,filename,conv);
except Exception:
	print "Failed to produce aov batching plot. continuing"

	
try:
	title="Effect of Different Input-Output Selections on Quality"
	conv = lambda x : get_path(plot,["selection","normal",x])
	indep = ["out","inout","rateout","strikeout","typeout","volout","timeout","all"]
	filename="batching.png"
	plot_info(title,indep,filename,conv);
except Exception:
	print "Failed to produce aov input-output plot. continuing"

