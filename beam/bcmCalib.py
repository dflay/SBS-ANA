#!/usr/bin/python
# A script to run BCM analysis

import os 
import sys

# TODO: 
# 1. read in a config file
# 2. make config file to each script below come from a JSON file (step 1) 

#===============================================================================
# process all cuts (on all BCM variables)  
varList    = ["unser","u1","unew","d1","d3","d10","dnew"]
confFile   = "./input/json/bcm-calib_02-05-22.json" # make this an input argument!
scriptName = "bcmCalibProcessCuts.cxx"
for var in varList: 
  cmd = "analyzer -q -b -l '{0}(\"{1}\",\"{2}\")'".format(scriptName,confFile,var)
  print(cmd)
  os.system(cmd) 
#===============================================================================
# compute BCM calibration coefficients 
