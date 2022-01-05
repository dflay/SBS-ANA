#!/bin/python
# Check the existence of a run in $OUT_DIR using a list of runs given by the user.
# Uses the ls command and counts how many file segments are in $OUT_DIR.  
# Assumes that the input file has a header line 
# Author: David Flay <flay@jlab.org>  

import os
import sys
import util_df

NARG = len(sys.argv)

if(NARG==2):
   inpath = sys.argv[1]
else:
   print("[get_replay_run_list]: ERROR!  Need one argument: input-file-path")
   print("                       input-file-path: path to file with run numbers (MUST contain a header line)")
   print("                       header must indicate which column the run is: run, Run, or RUN") 
   print("                       example: kin,run,info") 
   sys.exit(1)

print("Reading data from file: {0}".format(inpath) )

f = open(inpath,"r")

lines = f.readlines()
print("NL = {0}".format(len(lines)))

# find the run key 
runKey = util_df.getRunKey(lines[0]) 
 
entries = []
STR  = [] 
RUN  = [] 
NSEG = [] 

cntr=0
for line in lines:
   entries = line.split(',')
   if(cntr!=0):
      run = entries[runKey]
      cmd = "ls $OUT_DIR/*{0}* > test.txt".format(run)
      # print(cmd)
      os.system(cmd)
      # read back what we found 
      lsFile   = open("test.txt","r") 
      fileList = lsFile.readlines()
      NN       = len(fileList) 
      nSeg     = -1 
      if(NN!=0): 
         # find ROOT file name 
         parse    = fileList[0].split("/") # list 
         rfName   = parse[9] # string
         # find number of segments
         parse    = rfName.split("_") # list 
         nSeg_str = parse[5] # string
         parse    = nSeg_str.split(".") # list  
         nSeg     = parse[0]  # string 
         print( "Run {0:5d}: Found file: {1}, nseg = {2}".format(int(run),rfName,int(nSeg)) ) 
      # save results  
      RUN.append(run)
      NSEG.append(nSeg)    
   cntr = cntr + 1

# now print to a file 
outFile = open( "{0}_out".format(inpath),"w" ) 
NN = len(RUN) 
for i in xrange(0,NN): 
   outFile.write( "{0},{1},{2},{3}\n".format(RUN[i],0,0,NSEG[i]) )
outFile.close()  
print( "Printed results to: {0}_out".format(inpath) ) 

# clean up test output file 
os.system("rm test.txt") 
