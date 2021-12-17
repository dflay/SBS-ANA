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

