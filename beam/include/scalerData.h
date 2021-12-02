#ifndef UTIL_SCALER_DATA_H
#define UTIL_SCALER_DATA_H

// a data struct for scaler data 

typedef struct scalerData {
   std::string arm; 
   double time;  
   double unserRate;
   double unserCounts; 
   double u1Rate; 
   double u1Counts; 
   double unewRate; 
   double unewCounts; 
   double dnewRate; 
   double dnewCounts;
   double d1Rate; 
   double d1Counts; 
   double d3Rate; 
   double d3Counts; 
   double d10Rate; 
   double d10Counts; 
   int runNumber;
   int event;
   // constructor 
   scalerData():
      time(0),unserRate(0),unserCounts(0),u1Rate(0),u1Counts(0),unewRate(0),unewCounts(0),
      dnewRate(0),dnewCounts(0),d1Rate(0),d1Counts(0),d3Rate(0),d3Counts(0),d10Rate(0),d10Counts(0),
      runNumber(0),event(0)
   {}  
} scalerData_t; 

#endif 
