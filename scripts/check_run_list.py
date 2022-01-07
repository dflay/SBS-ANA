#!/bin/python
# Check the existence of a run in $DATA_DIR using a list of runs given by the user.
# Uses the ls command and counts how many file segments are in $DATA_DIR.  
# Assumes that the input file has a header line 
# Author: David Flay <flay@jlab.org>  

import os
import sys
import util_df

NARG = len(sys.argv)

if(NARG==2):
   inpath = sys.argv[1]
else:
   print("[check_run_list]: ERROR!  Need one argument: input-file-path")
   print("                  input-file-path: path to file with run numbers (MUST contain a header line)")
   print("                  header must indicate which column the run is: run, Run, or RUN")
   print("                  example: kin,run,info")
   sys.exit(1)

print("Reading data from file: {0}".format(inpath) )

f = open(inpath,"r")

lines = f.readlines()
print("NL = {0}".format(len(lines)))

# find the run key 
runKey = util_df.getRunKey(lines[0]) 

entries = []
RUN     = [] 
NSEG    = [] 

cntr=0
for line in lines:
   entries = line.split(',')
   run     = entries[runKey]
   if(cntr!=0):
      cmd = "ls $DATA_DIR/*_{0}* > test.txt".format(run)
      # print(cmd)
      os.system(cmd)
      # read back what we found 
      lsFile   = open("test.txt","r") 
      fileList = lsFile.readlines() 
      nSeg     = len(fileList) 
      print("Run {0:5d}: Found {1:3d} segments".format(int(run),int(nSeg) ) ) 
      # build an output file
      RUN.append(run) 
      NSEG.append(nSeg-1)  # nseg starts from 0!  
   cntr = cntr + 1

# now print to a file 
outFile = open( "{0}_out".format(inpath),"w" ) 
NN = len(RUN) 
for i in xrange(0,NN): 
   outFile.write( "{0},{1},{2},{3}\n".format(RUN[i],0,0,NSEG[i]) )
outFile.close()  

# clean up test output file 
os.system("rm test.txt") 
