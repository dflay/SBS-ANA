#ifndef UTIL_EPICS_DATA_H
#define UTIL_EPICS_DATA_H

// a simple struct for epics data 

typedef struct epicsData {
   std::string info;
   double time;
   double IBC1H04CRCUR2;
   double hac_bcm_average; 
   double IPM1H04A_XPOS; 
   double IPM1H04A_YPOS; 
   double IPM1H04E_XPOS; 
   double IPM1H04E_YPOS;
   int event;
   int runNumber;
   // constructor 
   epicsData():
      info("NONE"),time(0),IBC1H04CRCUR2(0),hac_bcm_average(0),
      IPM1H04A_XPOS(0),IPM1H04A_YPOS(0),
      IPM1H04E_XPOS(0),IPM1H04E_YPOS(0),
      event(0),runNumber(0)
   {} 
} epicsData_t; 

#endif 
