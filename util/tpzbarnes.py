#!/usr/bin/python

import sys
import math

def add_data(data, name, str, idx):
	nskipped = 0;
	styles = ['o', 'x', '^', '<', '>'];
	cols = ['r','b','g','o']
	i=0
	for line in str.splitlines():
		fields = line.split(',');
		if (len(fields) == 5 and i > 0):
			nums = map(float, fields);
			time = int(nums[0]);
			H1X = nums[2]; H1Y = nums[3]; H1Z=nums[4];
			if time not in data:
				data[time] = {'sim':{}};
				
			if name not in data[time]['sim']:
				data[time]['sim'][name] = {'x':[], 'y':[], 'z':[], 'color':cols[idx], 'marker':styles[idx]};
		
			if(math.isnan(H1X) or math.isnan(H1Y) or math.isnan(H1Z)):
				nskipped+=1;
			else:
				data[time]['sim'][name]['x'].append(H1X);
				data[time]['sim'][name]['y'].append(H1Y);
				data[time]['sim'][name]['z'].append(H1Z);
		i+=1	
	return nskipped;
	
res = {};
benchs = [];
n=len(sys.argv);
nskip=0;

out = sys.argv[1]
for i in range(2,n):
	file=open(sys.argv[i],"r");
	data=file.read();
	nskip+=add_data(res, sys.argv[i], data, i-2)
	file.close();
	
print "elements skipped: ",nskip
fout=open(out, "w+");
fout.write(str(res));
fout.close()
