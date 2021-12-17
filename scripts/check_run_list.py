#!/bin/python
# Check the existence of a run in $DATA_DIR using a list of runs given by the user.
# Uses the ls command and counts how many file segments are in $DATA_DIR.  
# Assumes that the input file has a header line 
# Author: David Flay <flay@jlab.org>  

import os
import sys

NARG = len(sys.argv)

if(NARG==3):
   inpath = sys.argv[1]
   runKey = int(sys.argv[2])
else:
   print("[get_run_list]: ERROR!  Need two arguments: input-file-path run-key")
   print("                input-file-path: absolute path to file with run numbers (must contain a header line)")
   print("                run-key: column index for run number")
   sys.exit(1)

print("Reading data from file: {0}".format(inpath) )
print("Run key: {0}".format(runKey) )

f = open(inpath,"r")

lines = f.readlines()
print("NL = {0}".format(len(lines)))

entries = []

RUN  = [] 
NSEG = [] 

cntr=0
for line in lines:
   entries = line.split(',')
   run     = entries[runKey]
   if(cntr!=0):
      cmd = "ls $DATA_DIR/*_{0}* > test.txt".format(run)
      # print(cmd)
      os.system(cmd)
      # read back what we found 
      lsFile = open("test.txt","r") 
      fileList = lsFile.readlines() 
      nSeg = len(fileList) 
      print("Run {0:5d}: Found {1:3d} segments".format(int(run),int(nSeg) ) ) 
      # build an output file
      RUN.append(run) 
      NSEG.append(nSeg)  
   cntr = cntr + 1

# now print to a file 
outFile = open( "{0}_out".format(inpath),"w" ) 
NN = len(RUN) 
for i in xrange(0,NN): 
   outFile.write( "{0},{1},{2},{3}\n".format(RUN[i],0,0,NSEG[i]) )
outFile.close()  

# clean up test output file 
os.system("rm test.txt") 
