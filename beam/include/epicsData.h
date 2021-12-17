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
   // get value of the member variable based on name 
   double getValue(std::string varName){
      double val=0;
      if(varName.compare("event")==0)           val = event;
      if(varName.compare("run")==0)             val = runNumber;
      if(varName.compare("time")==0)            val = time;
      if(varName.compare("IPM1H04A.XPOS")==0)   val = IPM1H04A_XPOS;
      if(varName.compare("IPM1H04A.YPOS")==0)   val = IPM1H04A_YPOS;
      if(varName.compare("IPM1H04E.XPOS")==0)   val = IPM1H04E_XPOS;
      if(varName.compare("IPM1H04E.YPOS")==0)   val = IPM1H04E_YPOS;
      if(varName.compare("hac_bcm_average")==0) val = hac_bcm_average;
      if(varName.compare("IBC1H04CRCUR2")==0)   val = IBC1H04CRCUR2;
      return val;
   }
   // print to screen 
   int Print(){
      std::cout << Form("event %03d, "             ,event)
	        << Form("run %05d, "               ,runNumber)
	        << Form("time = %.3lf, "           ,time)
	        << Form("IPM1H04A_XPOS = %.3lf, "  ,IPM1H04A_XPOS)
	        << Form("IPM1H04A_YPOS = %.3lf, "  ,IPM1H04A_YPOS)
	        << Form("IPM1H04E_XPOS = %.3lf, "  ,IPM1H04E_XPOS)
	        << Form("IPM1H04E_YPOS = %.3lf, "  ,IPM1H04E_YPOS)
	        << Form("hac_bcm_average = %.3lf, ",hac_bcm_average)
	        << Form("IPM1H04CRCUR2 = %.3lf, "  ,IBC1H04CRCUR2) << std::endl;
      return 0;
   } 
} epicsData_t; 

#endif 
