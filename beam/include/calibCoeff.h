#ifndef UTIL_CALIB_COEFF_H
#define UTIL_CALIB_COEFF_H

// a struct to store calibration coefficients 

typedef struct calibCoeff {
   std::string dev;
   double offset;
   double offsetErr;
   double slope;
   double slopeErr;
   calibCoeff(): 
      dev("NONE"),offset(0),offsetErr(0),slope(0),slopeErr(0)
   {}
} calibCoeff_t; 

#endif 
