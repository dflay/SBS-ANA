#!/bin/python
# A python wrapper script that recalls files from the mss system 
# over a list of runs given by the user in the input file 
# assumes that the input file has a header line 
# Author: David Flay <flay@jlab.org>  

import os
import sys
import util_df

NARG = len(sys.argv)

if(NARG==2):
   inpath = sys.argv[1]
else:
   print("[get_run_list]: ERROR!  Need one arguments: input-file-path")
   print("                input-file-path: absolute path to file with run numbers (MUST contain a header line)")
   print("                header must indicate which column the run is: run, Run, or RUN")
   print("                example: kin,run,info")
   sys.exit(1)

print("Reading data from file: {0}".format(inpath) )

f = open(inpath,"r")

lines = f.readlines()
print("NL = {0}".format(len(lines)))

runKey = util_df.getRunKey(lines[0]) 

entries = []

cntr=0
for line in lines:
   entries = line.split(',')
   run     = entries[runKey]
   if(cntr!=0):
      cmd = "./get_run.csh {0}".format(run)
      print(cmd)
      os.system(cmd)
   cntr = cntr + 1
