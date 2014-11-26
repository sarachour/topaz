#!/usr/bin/python

import sys
from os import listdir
from os.path import isfile, join
from copy import deepcopy;

def fill_energy_summary(row, path):
	handle = open(path);
	save=[0,1,10,11,12,13,14,15];
	i=0;
	for line in handle:
		tags = line.split(":");
		if(len(tags) == 2):
			if( i in save):
				name = tags[0].strip().replace(' ', '_');
				row['order'].append(name);
				row[name] = float(tags[1])
			i+=1;

def fill_output_summary(row, path):
	handle = open(path);
	i=0;
	row['order'].append('psnr');
	for line in handle:
		try:
			line = line.strip();
			if('inf' in line):
				row['psnr'] = 'inf';
			else:
				row['psnr'] = float(line)
				return;
			
		
		except ValueError:
			print 'error'
			pass;

def fill_task_statistics_summary(row, path):
	try:
		handle = open(path);
		save=[6,7,8,9,10,11];
		i=0;
		for line in handle:
			tags = line.split(":");
			
			if(len(tags) == 2):
				if( i in save):
					try:
						name = tags[0].strip().replace(' ', '_');
						row['order'].append(name);
						row[name] = float(tags[1])
					except ValueError:
						pass
				i+=1;
	except IOError:
		pass;
	
def build_dir_list(path):
	paths = {};
	for f in listdir(path):
		tags = f.split(".");
		strategy = tags[0];
		if(strategy != "correct"):
			machine = tags[len(tags) - 2];
		else:
			machine = "perfect"
		hasGod = True if 'god' in tags else False;
		hasTimelog = True if 'ltime' in tags else False;
		hasDetlog = True if 'ldet' in tags else False;
		isDet = True if 'ctrl' in tags else False;
		isPorous= True if 'pa' in tags else False;
		detTarg = 0;
		if(isDet):
			idx = tags.index('ctrl');
			detTarg = float('0.'+tags[idx+2])*100.0;
		key = f;
		paths[f] = {
					'order':['strategy','machine','input','det', 'porous', 'target'],
					'strategy':strategy,
					'machine':machine, 
					'input':'',
					'god':hasGod, 
					'time':hasTimelog, 
					'detlog':hasDetlog, 
					'det':isDet,
					'porous':isPorous,
					'target':detTarg,
					'path': path+"/"+f};
	return paths

def write_summary(pdata, fname):
	out = open(fname, "w+");
	header=False
	for r in pdata:
		row = pdata[r];
		if(header == False):
			for r in pdata:
				hdr = pdata[r]
				if hdr['strategy'] != 'correct':
					for elem in hdr['order']:
						out.write(elem+"\t");
					out.write("\n");
					header = True;
					break;
		
		for elem in row['order']:
			if(elem in row):
				out.write(str(row[elem])+"\t");
			else:
				out.write('\t');
		out.write("\n");
	
def build_energy_summary(pdata, fname):
	data = {};
	for dpath in pdata:
		orig_row = pdata[dpath];
		for fdir in listdir(orig_row['path']):
			tags = fdir.split('.');
			if(tags[0] == "timers"):
				dfile = dpath + "."+fdir;
				row = data[dfile] = deepcopy(orig_row);
				row['input'] = '.'.join(tags[1:])
				path = row['path']+"/"+fdir+"/"+"energy_summary.txt"
				fill_energy_summary(row, path);
				
	write_summary(data,fname);
	# print header
	
		
def build_output_summary(pdata, fname):
	data = {};
	for dpath in pdata:
		orig_row = pdata[dpath];
		for fdir in listdir(orig_row['path']):
			tags = fdir.split('.');
			if(tags[0] == "summary"):
				dfile = dpath + "."+fdir;
				row = data[dfile] = deepcopy(orig_row);
				row['input'] = '.'.join(tags[1:])
				path = row['path']+"/"+fdir
				fill_output_summary(row, path);
				
	write_summary(data,fname);
	# print header
def build_task_statistics_summary(pdata, fname):
	data = {};
	for dpath in pdata:
		orig_row = pdata[dpath];
		for fdir in listdir(orig_row['path']):
			tags = fdir.split('.');
			if(tags[0] == "logs" and orig_row['strategy'] != 'correct'):
				dfile = dpath + "."+fdir;
				row = data[dfile] = deepcopy(orig_row);
				row['input'] = '.'.join(tags[1:])
				path = row['path']+"/"+fdir+"/task_summary.0.txt";
				fill_task_statistics_summary(row, path);
				
	write_summary(data,fname);	
	

dirdata=build_dir_list("output")
build_energy_summary(dirdata, 'energy_summary.tab');
build_output_summary(dirdata, 'output_summary.tab');
build_task_statistics_summary(dirdata, 'task_stats_summary.tab');
