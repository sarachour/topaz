#!/usr/bin/python
import matplotlib.pyplot as plt
import numpy
import scipy


filename="data.txt"
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
		plot[typ][bs][prob]["low"] = numpy.std(data)
		plot[typ][bs][prob]["high"] = numpy.std(data)
		
		
	except ValueError:
		print "skipped ",line

	
def get(path):
	return get_path(plot,path);

title = "Tradeoff Curve for Bodytrack"
xlab = "Target Re-execution Rate"
ylab = "Relative Value"
xvals = [0,0.10,0.20,0.40,0.60,0.80,1.00]
filename = "tradeoff_curve_body.png"

fig,ax = plt.subplots()
axes = [ax, ax.twinx()]


fig.subplots_adjust(right=0.75);

axes[-1].spines['right'].set_position(('axes',1.2))
axes[-1].set_frame_on(True);
axes[-1].patch.set_visible(False);

plt.margins(0.05, 0.05)
plt.title(title,fontsize=14)
idxs = range(0,len(xvals));
qual = map(lambda x : get(["normal",5,x,"median"]),xvals) 
err = map(lambda x : get(["normal",5,x,"low"]),xvals) 
en = map(lambda x : get(["ltime",5,x,"median"]),xvals) 
errdet = map(lambda x : 100-get(["ldet",5,x,"median"]),xvals) 
w=3
colors = ["#2980b9","#27ae60","#c0392b"];
i=0;
#axes[i].plot(idxs,qual,label="Output Quality",color=colors[i],marker="o",linestyle="--");
axes[i].errorbar(idxs,qual,yerr=err,label="Output Quality",linewidth=w,color=colors[i],marker="o",linestyle="--");
axes[i].set_ylabel("Output Quality (% Pose Error)")
axes[i].spines['right'].set_color(colors[i]);

#i+=1;
#axes[i].plot(idxs,en,label="Energy Savings",linewidth=w, color=colors[i],marker="^",linestyle="-.");
#axes[i].set_ylabel("Energy Savings (%)")
#axes[i].spines['right'].set_color(colors[i]);

i+=1;
axes[i].plot(idxs,errdet,label="% Errors Detected",linewidth=w,color=colors[i],marker="x",linestyle=":");
axes[i].set_ylabel("% Errors Detected (%)")
axes[i].spines['right'].set_color(colors[i]);

axes[0].set_xlabel(xlab)
plt.xticks(idxs,xvals);
#plt.ylabel(indep)
#plt.barplot(indep, avg)
plt.savefig(filename)


title = "Output Quality / Taskset Quality Correlation for Bodytrack"
xlab = "Target Re-execution Rate"
ylab = "Relative Value"
xvals = [0,0.10,0.20,0.40,0.60,0.80,1.00]
filename = "correl_body.png"


fig,ax = plt.subplots()
axes = [ax, ax.twinx()]


fig.subplots_adjust(right=0.75);

axes[-1].spines['right'].set_position(('axes',1.2))
axes[-1].set_frame_on(True);
axes[-1].patch.set_visible(False);

plt.margins(0.05, 0.05)
plt.title(title,fontsize=14)
idxs = range(0,len(xvals));
qual = map(lambda x : get(["normal",5,x,"median"]),xvals) 
qual_err = map(lambda x : get(["normal",5,x,"low"]),xvals) 
task = map(lambda x : get(["ldeterr",5,x,"median"]),xvals) 
task_err = map(lambda x : get(["ldeterr",5,x,"low"]),xvals) 

colors = ["#2980b9","#c0392b", "#27ae60"];
w=3
i=0;
#axes[i].plot(idxs,qual,label="Output Quality",color=colors[i],marker="o",linestyle="--");
axes[i].errorbar(idxs,qual,label="Output Quality",yerr=qual_err,linewidth=w,color=colors[i],marker="o",linestyle="--");
axes[i].set_ylabel("Output Quality (% Pose Error)")
axes[i].spines['right'].set_color(colors[i]);

i+=1;
axes[i].plot(idxs,task,label="Taskset Quality", color=colors[i],linewidth=w,marker="^",linestyle="-.");
axes[i].set_ylabel("Taskset Quality (% Task Output Error)")
axes[i].spines['right'].set_color(colors[i]);

axes[0].set_xlabel(xlab)
plt.xticks(idxs,xvals);
plt.savefig(filename)

