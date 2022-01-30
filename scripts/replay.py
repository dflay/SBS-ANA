#!/bin/python
# Replay multiple runs from an input file  
# Author: David Flay <flay@jlab.org>  

import os
import sys
import datetime
import util_df  

isDebug = False 

NARG = len(sys.argv)

if(NARG==2):
   inpath = sys.argv[1]
else:
   print("[replay]: ERROR!  Need one argument: input-file-path")
   print("          input-file-path: path to file with run numbers (MUST contain a header line)") 
   print("          header must indicate which column the run is: run, Run, or RUN")
   print("          example: kin,run,info")
   sys.exit(1)

# get the date 
theDate = util_df.getDate()  
theTime = util_df.getTime()  

print("Reading data from file: {0}".format(inpath) )
f = open(inpath,"r")

lines = f.readlines()
print("NL = {0}".format(len(lines)))

# find the run key 
runKey = util_df.getRunKey(lines[0])

entries = []

# default arguments to the replay script
firstEvent = 0 
lastEvent  = -1 
nSegments  = 700 

# files to save status
# chop off the file extension and use the name for the log files
strList     = inpath.split(".")   
logFile     = open("{0}.log".format(strList[0])    ,"a")
summaryFile = open("{0}.summary".format(strList[0]),"a")

# loop over all runs 
cntr=0
for line in lines:
   entries = line.split(',')
   run     = entries[runKey]
   if(cntr!=0):
      currentTime = util_df.getTime()
      tag = "[{0} {1}]".format(theDate,currentTime) 
      cmd = "analyzer -b -q 'replay_beam.C({0},{1},{2},{3})' 2>&1 | tee ./log/run-{0}.log".format(run,firstEvent,lastEvent,nSegments)
      msg = "{0}: executing: {1}\n".format(tag,cmd) 
      logFile.write(msg)
      print(cmd)
      if(isDebug==False): 
         os.system(cmd)
      currentTime = util_df.getTime()
      tag = "[{0} {1}]".format(theDate,currentTime) 
      msg = "{0}: replay for run {1} complete!\n".format(tag,run)  
      logFile.write(msg)
      summaryFile.write( "{0}\n".format(run) )
   cntr = cntr + 1

# close log files 
logFile.close()
summaryFile.close()
