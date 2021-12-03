#ifndef UTIL_SCALER_DATA_H
#define UTIL_SCALER_DATA_H

// a data struct for scaler data 

typedef struct scalerData {
   std::string arm;       // arm (Left, sbs, or E)  
   double time;           // time in seconds                    
   double time_num;       // time numerator: clock counts                
   double time_den;       // time denominator: clock rate
   double unserRate;      // Unser rate                  
   double unserCounts;    // Unser counts                
   double u1Rate;         // upstream BCM x1 (analog) rate           
   double u1Counts;       // upstream BCM x1 (analog) counts           
   double unewRate;       // upstream BCM x1 (digital) rate               
   double unewCounts;     // upstream BCM x1 (digital) counts                
   double dnewRate;       // downstream BCM x1 (digital) rate               
   double dnewCounts;     // downstream BCM x1 (digital) counts                
   double d1Rate;         // downstream BCM x1 (analog) rate                 
   double d1Counts;       // downstream BCM x1 (analog) counts               
   double d3Rate;         // downstream BCM x3 (analog) rate             
   double d3Counts;       // downstream BCM x3 (analog) counts            
   double d10Rate;        // downstream BCM x10 (analog) rate            
   double d10Counts;      // downstream BCM x10 (analog) counts        
   int runNumber;
   int event;
   // constructor 
   scalerData():
      time(0),time_num(0),time_den(0),
      unserRate(0),unserCounts(0),
      u1Rate(0),u1Counts(0),unewRate(0),unewCounts(0),
      dnewRate(0),dnewCounts(0),d1Rate(0),d1Counts(0),d3Rate(0),d3Counts(0),d10Rate(0),d10Counts(0),
      runNumber(0),event(0)
   {}  
} scalerData_t; 

#endif 
