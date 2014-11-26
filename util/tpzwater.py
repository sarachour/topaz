#!/usr/bin/python

import sys
import math

def add_data(data, name, str, idx):
	nskipped = 0;
	styles = ['o', 'x', '^', '<', '>'];
	
	for line in str.splitlines():
		fields = line.split('\t');
		if (len(fields) == 10):
			nums = map(float, fields);
			time = int(nums[0]);
			H1X = nums[1]; H1Y = nums[2]; H1Z=nums[3];
			OX = nums[4]; OY = nums[5]; OZ=nums[6];
			H2X = nums[7]; H2Y = nums[8]; H2Z=nums[9];
			if time not in data:
				#data[time] = {'H1':{}, 'H2':{}, 'O':{}};
				data[time] = { 'O':{}};
			if name not in data[time]['O']:
				#data[time]['H1'][name] = {'x':[], 'y':[], 'z':[], 'color':'b', 'marker':styles[idx]};
				#data[time]['H2'][name] = {'x':[], 'y':[], 'z':[], 'color':'b', 'marker':styles[idx]};
				data[time]['O'][name] = {'x':[], 'y':[], 'z':[], 'color':'r', 'marker':styles[idx]};
		
			#if(math.isnan(H1X) or math.isnan(H1Y) or math.isnan(H1Z)):
		    #		nskipped+=1;
			#else:
			#	data[time]['H1'][name]['x'].append(H1X);
			#	data[time]['H1'][name]['y'].append(H1Y);
			#	data[time]['H1'][name]['z'].append(H1Z);
				
			#if(math.isnan(H2X) or math.isnan(H2Y) or math.isnan(H2Z)):
			#	nskipped+=1;
			#else:
			#	data[time]['H2'][name]['x'].append(H2X);
			#	data[time]['H2'][name]['y'].append(H2Y);
			#	data[time]['H2'][name]['z'].append(H2Z);
			
			if(math.isnan(OX) or math.isnan(OY) or math.isnan(OZ)):
				nskipped+=1;
			else:
				data[time]['O'][name]['x'].append(OX);
				data[time]['O'][name]['y'].append(OY);
				data[time]['O'][name]['z'].append(OZ);
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
