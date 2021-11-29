#ifndef UTIL_CUT_H 
#define UTIL_CUT_H 

// a simple struct to hold cut data 

typedef struct cut {
   std::string dev;            // device name 
   std::string beam_state;     // beam state (on or off) 
   double low;                 // low cut value 
   double high;                // high cut value 
   int group;                  // group/categorization 
   // constructor 
   cut():
      dev("NONE"),beam_state("NONE"),low(0),high(0),group(0) 
   {} 
} cut_t; 

#endif 
