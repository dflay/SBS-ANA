#!/bin/csh 
# Use jcache to get a raw (EVIO) file from tape 
# and store to the raw data directory on the farm
# Author: David Flay <flay@jlab.org>  

# RAW_DATA_DIR=/cache/halla/sbs/raw
setenv RAW_DATA_DIR ${DATA_DIR}
setenv MSS_DIR "/mss/halla/sbs/raw"
setenv RUN_NUMBER $1

echo "Moving to ${RAW_DATA_DIR}..."
cd ${RAW_DATA_DIR}
echo "Submitting request..."
jcache get ${MSS_DIR}/e1209019_${RUN_NUMBER}.evio.* 
echo "--> Done."

