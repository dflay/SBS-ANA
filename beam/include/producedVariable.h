#ifndef UTIL_PRODUCED_VARIABLE_H 
#define UTIL_PRODUCED_VARIABLE_H 

// a simple struct to hold produced data  

typedef struct producedVariable {
   std::string dev;            // device name 
   std::string beam_state;     // beam state (on or off) 
   double mean;                // mean 
   double stdev;               // stdev 
   int group;                  // group/categorization 
   // constructor 
   producedVariable():
      dev("NONE"),beam_state("NONE"),mean(0),stdev(0),group(0) 
   {} 
} producedVariable_t; 

#endif 
