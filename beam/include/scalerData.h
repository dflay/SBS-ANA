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
   double unserCurrent;   // Unser current                
   double u1Rate;         // upstream BCM x1 (analog) rate           
   double u1Counts;       // upstream BCM x1 (analog) counts           
   double u1Current;      // upstream BCM x1 (analog) current           
   double unewRate;       // upstream BCM x1 (digital) rate               
   double unewCounts;     // upstream BCM x1 (digital) counts                
   double unewCurrent;    // upstream BCM x1 (digital) current                
   double dnewRate;       // downstream BCM x1 (digital) rate               
   double dnewCounts;     // downstream BCM x1 (digital) counts                
   double dnewCurrent;    // downstream BCM x1 (digital) current               
   double d1Rate;         // downstream BCM x1 (analog) rate                 
   double d1Counts;       // downstream BCM x1 (analog) counts               
   double d1Current;      // downstream BCM x1 (analog) current               
   double d3Rate;         // downstream BCM x3 (analog) rate             
   double d3Counts;       // downstream BCM x3 (analog) counts            
   double d3Current;      // downstream BCM x3 (analog) current        
   double d10Rate;        // downstream BCM x10 (analog) rate            
   double d10Counts;      // downstream BCM x10 (analog) counts       
   double d10Current;     // downstream BCM x10 (analog) current        
   int runNumber;
   int event;
   // constructor 
   scalerData():
      time(0),time_num(0),time_den(0),
      unserRate(0),unserCounts(0),unserCurrent(0),
      u1Rate(0),u1Counts(0),u1Current(0),unewRate(0),unewCounts(0),unewCurrent(0),
      dnewRate(0),dnewCounts(0),dnewCurrent(0),d1Rate(0),d1Counts(0),d1Current(0),
      d3Rate(0),d3Counts(0),d3Current(0),d10Rate(0),d10Counts(0),d10Current(0),
      runNumber(0),event(0)
   {}  
} scalerData_t; 

#endif 
