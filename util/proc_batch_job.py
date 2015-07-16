#!/usr/bin/python
import matplotlib.pyplot as plt
import numpy
import scipy

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
		
		typ=fields[2]
		bs=int(fields[1])
		prob=float(fields[0])
		data=[]
		
		
		for i in range(3,len(fields)):
			try:
				data.append(float(fields[i]))
			except ValueError:
				print "continuing."
			
		make_path(plot, [typ,bs,prob],{});
		#stdev = scipy.std(data)
		avg = numpy.percentile(data, 50)
		plot[typ][bs][prob]["median"] =avg 
		plot[typ][bs][prob]["low"] = avg-numpy.percentile(data, 25)
		plot[typ][bs][prob]["high"] = numpy.percentile(data, 75)-avg
		
		
	except ValueError:
		print "skipped ",line

def plot_figure(filename, title, xaxis, yaxis, xvals, xticks, funx,funy):
	plt.figure()
	plt.margins(0.05, 0.05)
	plt.title(title)
	indep = xvals
	indep_num = range(0,len(indep));
	data= map(lambda x : funx(x),indep)
	avg = map(lambda x : funy(x["median"]),data) 
	lw = map(lambda x : (x["low"]),data) 
	hi = map(lambda x : (x["high"]),data) 
	
	plt.errorbar(indep_num, avg, yerr=[lw,hi], fmt='o--')
	plt.xlabel(xaxis)
	plt.ylabel(yaxis)
	plt.xticks(indep_num,xticks);
	#plt.ylabel(indep)
	#plt.barplot(indep, avg)
	plt.savefig(filename)
	
def get(path):
	return get_path(plot,path);

xvals = [1,2,3,4]
xticks = xvals
xlab = "Number of Blocks"
xfun = lambda x : get(['normal',x+1,0.01])

try:
	title = "Block Size vs Percent Error (Quality)"
	ylab = "Percent Errors"
	yfun = lambda y : y
	xfun = lambda x : get(['normal',5,x])
	filename = "qual_bs.png"
	plot_figure(filename, title, xlab, ylab, xvals, xticks, xfun, yfun);
except KeyError:
	print "Failed to produce qual bs plot. continuing"

try:
	title = "Block Size vs Errors Detected (Detector)"
	ylab = "Percent Errors Detected"
	yfun = lambda y : 100-y
	xfun = lambda x : get(['ldet',5,x])
	filename = "det_bs.png"
	plot_figure(filename, title, xlab, ylab, xvals, xticks, xfun, yfun);
except KeyError:
	print "Failed to produce det bs plot. continuing"
	
try:
	title = "Block Size vs Percent Energy Savings (Energ)"
	ylab = "Percent Energy Savings"
	yfun = lambda y : y
	xfun = lambda x : get(['ltime',5,x])
	filename = "en_bs.png"
	plot_figure(filename, title, xlab, ylab, xvals, xticks, xfun, yfun);
except KeyError:
	print "Failed to produce en bs plot. continuing"
	

xvals = [0,0.01,0.02,0.04]
xticks = xvals
xlab = "Target Re-Execution Rate"
xfun = lambda x : get(['normal',5,x])

try:
	title = "Target Re-Execution Rate vs Percent Error (Quality)"
	ylab = "Percent Errors"
	yfun = lambda y : y
	xfun = lambda x : get(['normal',5,x])
	filename = "qual_prob.png"
	plot_figure(filename, title, xlab, ylab, xvals, xticks, xfun, yfun);

except KeyError:
	print "Failed to produce qual prob plot. continuing"

try:
	title = "Target Re-Execution Rate vs Percent Errors Detected (Detector)"
	ylab = "Percent Errors Detected"
	yfun = lambda y : 100-y
	xfun = lambda x : get(['ldet',5,x])
	filename = "det_prob.png"
	plot_figure(filename, title, xlab, ylab, xvals, xticks, xfun, yfun);

except KeyError:
	print "Failed to produce det prob plot. continuing"


try:
	title = "Target Re-Execution Rate vs Percent Energy Savings (Energy)"
	ylab = "Percent Energy Savings"
	yfun = lambda y : y
	xfun = lambda x : get(['ltime',5,x])
	filename = "en_prob.png"
	plot_figure(filename, title, xlab, ylab, xvals, xticks, xfun, yfun);

except KeyError:
	print "Failed to produce en prob plot. continuing"

xvals = [0,0.10,0.20,0.40,0.60,0.80,1.00]
xticks = xvals
xlab = "Target Re-Execution Rate"

try:
	title = "Target Probability vs Percent Error (Quality)"
	ylab = "Percent Errors"
	yfun = lambda y : y
	xfun = lambda x : get(['normal',5,x])
	filename = "qual_macro_prob.png"
	plot_figure(filename, title, xlab, ylab, xvals, xticks, xfun, yfun);

except KeyError:
	print "Failed to produce  macro qual prob plot. continuing"

try:
	title = "Target Probability vs Percent Errors Detected (Detect)"
	ylab = "Percent Errors Detected"
	yfun = lambda y : y
	xfun = lambda x : get(['ldet',5,x])
	filename = "det_macro_prob.png"
	plot_figure(filename, title, xlab, ylab, xvals, xticks, xfun, yfun);

except KeyError:
	print "Failed to produce  macro det prob plot. continuing"

try:
	title = "Target Probability vs Percent Energy Savings (Energy)"
	ylab = "Percent Energy Savings"
	yfun = lambda y : y
	xfun = lambda x : get(['ltime',5,x])
	filename = "en_macro_prob.png"
	plot_figure(filename, title, xlab, ylab, xvals, xticks, xfun, yfun);

except KeyError:
	print "Failed to produce  macro en prob plot. continuing"
