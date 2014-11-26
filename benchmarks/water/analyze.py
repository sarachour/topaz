import time                     # Imports system time module to time your script
from scipy import stats as st
import numpy as np
import csv 
import sys                     # Imports .csv file reader

# Read in small data from .csv file
# Filepath
# In windows you can also specify the absolute path to your data file
# filepath = 'C:/Dropbox/Towson/Teaching/3_ComputationalEconomics/Lectures/Lecture4/'

# ------------- Load data --------------------
data1 =[]  # Define empty list for data reading
data2 =[]  # Define empty list for data reading
tcnt=0
fcnt=0
fname1=sys.argv[1]
fname2=sys.argv[2]
file1 = open(fname1,'r')
file2 = open(fname2,'r')
for row in file1:
		try:
			carr=np.array(row.rstrip().split('\t'), dtype=np.double);
			farr =carr.astype(np.double);
			data1.append((farr))    # read data row by row
			tcnt+=1
		except:
			fcnt+=1
			
for row in file2:
		try:
			carr=np.array(row.rstrip().split('\t'), dtype=np.double);
			farr =carr.astype(np.double);
			data2.append((farr))    # read data row by row
			tcnt+=1
		except:
			fcnt+=1
# Let's have a look at it, it's a nested list
print "Successfully Read: %d, Failed on: %d\n" % (tcnt, fcnt)

diff = np.abs(np.subtract(data1, data2));
sumsq = np.sum(np.power(diff,2), axis=1)
dist = np.sqrt(sumsq);
print "mean %e\n" % np.mean(dist, axis=0)
print "std %e\n" % np.std(dist, axis=0)
print "max %e\n" % np.max(dist, axis=0)
print "Bounding Box:"
print np.min(data1, axis=0)
print np.max(data1, axis=0)


