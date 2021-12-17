#ifndef UTIL_CALIB_COEFF_H
#define UTIL_CALIB_COEFF_H

// a struct to store calibration coefficients 

typedef struct calibCoeff {
   std::string dev;      // name of device: unser, u1, unew, d1, d3, d10, dnew
   std::string info;     // user info 
   double pedestal;      // pedestal (beam off) [Hz] 
   double pedestalErr;   // pedestal (beam off) error [Hz]
   double offset;        // fitted offset [Hz]  
   double offsetErr;     // fitted offset error [Hz]   
   double slope;         // fitted slope  [Hz/uA] 
   double slopeErr;      // fitted slope error [Hz/uA] 
   int runMin;           // run range: minimum    
   int runMax;           // run range: maximum   
   calibCoeff(): 
      dev("NONE"),info("NONE"),
      pedestal(0),pedestalErr(0),
      offset(0),offsetErr(0),
      slope(0),slopeErr(0),
      runMin(0),runMax(0)
   {}
} calibCoeff_t; 

#endif 
