#!/bin/csh 

echo "Setting up SBS analysis environment..."
setenv SBS_REPLAY '/w/halla-scshelf2102/sbs/dflay/SBS-replay'
setenv DB_DIR ${SBS_REPLAY}/DB
setenv DATA_DIR /cache/halla/sbs/raw/
setenv ANALYZER_CONFIGPATH ${SBS_REPLAY}/replay
setenv OUT_DIR /lustre19/expphy/volatile/halla/sbs/flay/GMnAnalysis/rootfiles
setenv LOG_DIR /lustre19/expphy/volatile/halla/sbs/flay/GMnAnalysis/logs

echo "SBS_REPLAY =" ${SBS_REPLAY} 
echo "DB_DIR =" ${DB_DIR} 
echo "ANALYZER_CONFIGPATH =" ${ANALYZER_CONFIGPATH} 
echo "OUT_DIR =" ${OUT_DIR} 
echo "LOG_DIR =" ${LOG_DIR} 
echo "--> Done!" 
