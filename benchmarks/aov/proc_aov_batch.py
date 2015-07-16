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


def plot_info(title,indep_data,filename,xaxis,yaxis,ylabels,delta,conv):
	plt.figure()
	plt.margins(0.05, 0.05)
	plt.title(title)
	indep = indep_data
	indep_num = range(0,len(indep_data));
	data= map(lambda x : conv(x),indep)
	avg = map(lambda x : delta(x["median"]),data) 
	lw = map(lambda x : (x["low"]),data) 
	hi = map(lambda x : (x["high"]),data) 
	
	plt.errorbar(indep_num, avg, yerr=[lw,hi], fmt='o--')
	plt.xlabel(xaxis)
	plt.ylabel(yaxis)
	plt.xticks(indep_num,indep);
	#plt.ylabel(indep)
	#plt.barplot(indep, avg)
	plt.savefig(filename)
	
print plot

xaxis="Normalization Type"
indep = ["naive","subtract","arbitrarge"]
labels = ["price", "price-initial_price", "arbitrarge"]
try:
	title="Effect of Different Normalization Algorithms on Quality"
	conv = lambda x : get_path(plot,["normalization","normal",x])
	filename="norm.png"
	yaxis="Percent Error"
	fun = lambda x : x
	plot_info(title,indep,filename,xaxis,yaxis,labels,fun,conv);
except KeyError:
	print "Failed to produce aov normalization plot. continuing"

try:
	title="Effect of Different Normalization Algorithms on Energy"
	conv = lambda x : get_path(plot,["normalization","ltime",x])
	filename="norm-energy.png"
	yaxis="Energy Savings"
	fun = lambda x : x
	plot_info(title,indep,filename,xaxis,yaxis,labels,fun,conv);
except KeyError:
	print "Failed to produce aov normalization energy plot. continuing"

try:
	title="Effect of Different Normalization Algorithms on Error Detection"
	conv = lambda x : get_path(plot,["normalization","ldet",x])
	indep = ["naive","subtract","arbitrarge"]
	filename="norm-detect.png"
	xaxis="Normalization Type"
	yaxis="Percent Errors Uncaught"
	plot_info(title,indep,filename,xaxis,yaxis,labels,fun,conv);
except KeyError:
	print "Failed to produce aov normalization error detection plot. continuing"


	

indep = ["batch1","batch2","batch4","batch8"]
labels = ["1", "2", "3", "4"]
xaxis="Number of Prices per AOV element"
	
try:
	title="Effect of Different Batch Sizes on Quality"
	conv = lambda x : get_path(plot,["batching","normal",x])
	filename="batching.png"
	yaxis="Percent Price Error"
	fun = (lambda x : x)
	plot_info(title,indep,filename,xaxis,yaxis,labels,fun,conv);
except Exception:
	print "Failed to produce aov batching plot. continuing"

try:
	title="Effect of Different Batch Sizes on Energy"
	conv = lambda x : get_path(plot,["batching","ltime",x])
	filename="batching-energy.png"
	yaxis="Energy Savings"
	fun = (lambda x : x)
	plot_info(title,indep,filename,xaxis,yaxis,labels,fun,conv);
except Exception:
	print "Failed to produce aov batching energy plot. continuing"

try:
	title="Effect of Different Batch Sizes on Error Detection"
	conv = lambda x : get_path(plot,["batching","ldet",x])
	filename="batching-detect.png"
	yaxis="Percent Errors Detected"
	fun = (lambda x : 100-x)
	plot_info(title,indep,filename,xaxis,yaxis,labels,fun,conv);
except Exception:
	print "Failed to produce aov batching detection plot. continuing"

	
indep = ["out","inout","rateout","strikeout","typeout","volout","timeout","all"]
labels = ["price", "initial price,price","rate,price", "strike,price","type,price","volatility,price","time,price","all"]		
xaxis="Tuple Elements Selected"
try:
	title="Effect of Different Input-Output Selections on Quality"
	conv = lambda x : get_path(plot,["selection","normal",x])
	filename="selection.png"
	yaxis="Percent Price Error"
	fun = (lambda x : x);
	plot_info(title,indep,filename,xaxis,yaxis,labels,fun,conv);
except Exception:
	print "Failed to produce aov input-output plot. continuing"
	
try:
	title="Effect of Different Input-Output Selections on Energy"
	conv = lambda x : get_path(plot,["selection","ltime",x])
	filename="selection-energy.png"
	yaxis="Percent Energy Savings"
	fun = (lambda x : x);
	plot_info(title,indep,filename,xaxis,yaxis,labels,fun,conv);
except Exception:
	print "Failed to produce aov input-output energy plot. continuing"

try:
	title="Effect of Different Input-Output Selections on Error Detection"
	conv = lambda x : get_path(plot,["selection","ldet",x])
	filename="selection-detect.png"
	yaxis="Percent Errors Detected"
	fun = (lambda x : 100-x);
	plot_info(title,indep,filename,xaxis,yaxis,labels,fun,conv);
except Exception:
	print "Failed to produce aov input-output detection plot. continuing"


