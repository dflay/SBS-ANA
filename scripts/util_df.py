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
