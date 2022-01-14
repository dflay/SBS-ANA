#!/bin/python
# utility functions  
# Author: David Flay <flay@jlab.org>  

import os
import sys
import datetime 

#_______________________________________________________________________________
def getDate(): 
   ts = datetime.datetime.now()
   theDate = "{0:4d}-{1:02d}-{2:02d}".format(ts.year,ts.month,ts.day)
   return theDate
#_______________________________________________________________________________
def getTime(): 
   ts = datetime.datetime.now() 
   theTime = "{0:02d}:{1:02d}:{2:02d}".format(ts.hour,ts.minute,ts.second)
   return theTime
#_______________________________________________________________________________
def getNumSegments(dirPath,run):
   # a simple way to count the number of file segments for a CODA run 
   # typically stored under /mss/ 
   cmd = "ls {0}/*_{1}* > test.txt".format(dirPath,run)
   # print(cmd)
   os.system(cmd)
   # read back what we found 
   lsFile   = open("test.txt","r")
   fileList = lsFile.readlines()
   nSeg     = len(fileList)
   print( "[util_df::getNumSegments]: Run {0:5d}: Found {1:3d} segments".format( int(run),int(nSeg) ) )
   # clean up test output file 
   os.system("rm test.txt")
   return nSeg
#_______________________________________________________________________________
def getRunKey(inputStr):
   # find the index that matches the pattern indicated below
   # and return that index 
   # input string must be comma separated  
   runKey = -1
   keys   = inputStr.split(',')
   M      = len(keys)
   for i in xrange(0,M):
      if(keys[i]=="run" or keys[i]=="Run" or keys[i]=="RUN"):
         runKey = i
         print( "[util_df::getRunKey]: FOUND: run key = {0}".format(runKey) )
   return runKey 
