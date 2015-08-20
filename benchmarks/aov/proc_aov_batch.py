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
		prob=float(fields[3])
		bs=int(fields[4])
		typ=fields[5]
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
	try:
		title = title
		xlab = axis
		filename = filename+".png"
		FONTSIZE=24
		LABSIZE=16
		LEGSIZE=20
		
		fig,ax = plt.subplots()
		axes = [ax, ax.twinx(), ax.twinx()]
		#axes = [ax, ax.twinx()]


		fig.subplots_adjust(right=1);

		axes[-1].spines['right'].set_position(('axes',1.2))
		axes[-1].set_frame_on(True);
		axes[-1].patch.set_visible(False);

		plt.margins(0.05, 0.05)
		#plt.title(title,fontsize=FONTSIZE)
		idxs = range(0,len(xvals));
		qual = map(lambda x : get([key,"normal",x,"median"]),xvals) 
		en = map(lambda x : get([key,"ltime-tpz",x,"median"]),xvals) 
		det = map(lambda x : get([key,"ldet-errs",x,"median"]),xvals) 

		colors = ["#2980b9","#27ae60","#c0392b"];
		lw=5
		i=0;
		axes[i].plot(idxs,qual,label="Relative Price Error (%)",linewidth=lw,linestyle="-",color=colors[i],marker="^");
		axes[i].set_ylabel("Relative Price Error (%)",fontsize=FONTSIZE)

		i+=1;
		axes[i].plot(idxs,en,label="Energy Savings (%)", linewidth=lw,linestyle="--",color=colors[i],marker="h");
		axes[i].set_ylabel("Energy Savings (%)",fontsize=FONTSIZE )

		i+=1;
		axes[i].plot(idxs,det,label="Error Detection Rate (%)",linewidth=lw,linestyle=":",color=colors[i],marker="o");
		axes[i].set_ylim(0,axes[i].get_ylim()[1]);
		axes[i].set_ylabel("Error Detection Rate (%)",fontsize=FONTSIZE)

		axes[0].set_xlabel(xlab,fontsize=FONTSIZE)
		
		handles = [];
		labels = [];
		h, l = axes[0].get_legend_handles_labels()
		handles = handles + h
		labels = labels+l
		h, l = axes[1].get_legend_handles_labels()
		handles = handles + h
		labels = labels+l
		h, l = axes[2].get_legend_handles_labels()
		handles = handles + h
		labels = labels+l
		
		print handles,labels
		lgd = plt.legend(handles, labels,loc="upper center", bbox_to_anchor=(0.5,-0.2),ncol=3, fontsize=LEGSIZE)
		
		
		axes[0].tick_params(axis="y",labelsize=LABSIZE);
		axes[1].tick_params(axis="y",labelsize=LABSIZE);
		axes[2].tick_params(axis="y",labelsize=LABSIZE);
		if(is_tiny):
			axes[0].set_xticks(map(lambda x : x, idxs), minor=False);
			axes[0].set_xticklabels(xticks,rotation=45);
			axes[0].tick_params(axis="x",labelsize=LABSIZE);
		
		else:
			axes[0].set_xticks(map(lambda x : x, idxs), minor=False);
			axes[0].set_xticklabels(xticks,rotation=0);
			axes[0].tick_params(axis="x",labelsize=LABSIZE);
		
                
		plt.savefig(filename, bbox_extra_artists=(lgd,), bbox_inches='tight')
		
	except KeyError:
		print "Skipping"
print plot

indep = ["out","inout","rateout","strikeout","typeout","volout","timeout","all"]
labels = ["price", "input","rate", "strike","type","vol","time","all"]		
xaxis="Tuple Elements Selected"
title="Effect of Input-Output Tuple Selection"
produce_tuple_plot("tradeoff_selection",title,"selection",indep,labels,xaxis,True)


indep = ["batch1","batch2","batch4","batch8"]
labels = ["1", "2", "4","8"]
xaxis="Dimensionality of AOV"
title="Effect of Reducing Outputs"
produce_tuple_plot("tradeoff_batching",title,"batching",indep,labels,xaxis,False)



xaxis="Normalization Type"
indep = ["naive","arbitrarge"]
labels = ["price", "arbitrarge"]
title="Effect of Price Normalization"
produce_tuple_plot("tradeoff_norm",title,"normalization",indep,labels,xaxis,False)
	
