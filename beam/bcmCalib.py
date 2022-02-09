#!/usr/bin/python
# A script to run BCM analysis

import os 
import sys

# read in a config file!

varList = ["unser","u1","unew","d1","d3","d10","dnew"]

#===============================================================================
# process all cuts 
confFile = "./input/json/bcm-calib_02-05-22.json" # make this an input argument!

scriptName = "bcmCalibProcessCuts.cxx"
for var in varList: 
  cmd = "analyzer -q -b -l '{0}(\"{1}\",\"{2}\")'".format(scriptName,confFile,var)
  print(cmd)
  os.system(cmd) 

#===============================================================================
# compute BCM calibration coefficients 
