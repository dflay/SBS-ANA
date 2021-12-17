#!/bin/python
# A python wrapper script that recalls files from the mss system 
# over a list of runs given by the user in the input file 
# assumes that the input file has a header line 
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

cntr=0
for line in lines:
   entries = line.split(',')
   run     = entries[runKey]
   if(cntr!=0):
      cmd = "./get_run.csh {0}".format(run)
      print(cmd)
      os.system(cmd)
   cntr = cntr + 1
