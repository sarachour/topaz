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
	print elems
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



def get(path):
	return get_path(plot,path);

def produce_tuple_plot(filename,title,key,xvals,xticks,axis,is_tiny):
	title = title
	xlab = axis
	filename = filename+".png"
	
	fig,ax = plt.subplots()
	axes = [ax, ax.twinx(), ax.twinx()]
	#axes = [ax, ax.twinx()]


	fig.subplots_adjust(right=0.75);

	axes[-1].spines['right'].set_position(('axes',1.2))
	axes[-1].set_frame_on(True);
	axes[-1].patch.set_visible(False);

	plt.margins(0.05, 0.05)
	plt.title(title,fontsize=14)
	idxs = range(0,len(xvals));
	qual = map(lambda x : get([key,"normal",x,"median"]),xvals) 
	en = map(lambda x : get([key,"ltime",x,"median"]),xvals) 
	det = map(lambda x : get([key,"ldet",x,"median"]),xvals) 

	colors = ["#2980b9","#27ae60","#c0392b"];
	barw = 0.25;
	i=0;
	axes[i].bar(map(lambda x: x + barw*i, idxs),qual,barw,label="Output Quality",color=colors[i],hatch="/");
	axes[i].set_ylabel("Output Quality (Cluster Score)")
	axes[i].spines['left'].set_color(colors[i]);

	i+=1;
	axes[i].bar(map(lambda x: x + barw*i, idxs),en,barw,label="Energy Savings", color=colors[i],hatch="o");
	axes[i].set_ylabel("Energy Savings (%)")
	axes[i-1].spines['right'].set_color(colors[i]);

	i+=1;
	axes[i].bar(map(lambda x: x + barw*i, idxs),det,barw,label="% Errors Detected",color=colors[i],hatch="-");
	axes[i].set_ylim(0,axes[i].get_ylim()[1]);
	axes[i].set_ylabel("% Errors Detected (%)")
	axes[i].spines['right'].set_color(colors[i]);

	axes[0].set_xlabel(xlab)
	
	if(is_tiny):
		axes[0].set_xticks(map(lambda x : x + 0.5, idxs), minor=False);
		axes[0].set_xticklabels(xticks,rotation=45);
		axes[0].tick_params(axis="x",labelsize=8);
	
	else:
		axes[0].set_xticks(map(lambda x : x + 0.5, idxs), minor=False);
		axes[0].set_xticklabels(xticks,rotation=0);
		
	plt.savefig(filename)

print plot

indep = ["out","inout","rateout","strikeout","typeout","volout","timeout","all"]
labels = ["price", "input","rate", "strike","type","vol","time","all"]		
xaxis="Tuple Elements Selected"
title="Effect of Input-Output Tuple Selection"
produce_tuple_plot("tradeoff_selection",title,"selection",indep,labels,xaxis,True)


indep = ["batch1","batch2","batch4","batch8"]
labels = ["1", "2", "4","8"]
xaxis="Number of Prices per AOV element"
title="Effect of Batching Outputs"
produce_tuple_plot("tradeoff_batching",title,"batching",indep,labels,xaxis,False)



xaxis="Normalization Type"
indep = ["naive","arbitrarge"]
labels = ["price", "arbitrarge"]
title="Effect of Price Normalization"
produce_tuple_plot("tradeoff_norm",title,"normalization",indep,labels,xaxis,False)
	
