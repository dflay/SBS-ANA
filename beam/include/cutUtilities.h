#ifndef CUT_UTIL_H
#define CUT_UTIL_H 

// a namespace for helpful functions useful in 
// BCM data analysis 

#include <cstdlib>
#include <iostream>
#include <vector> 

#include "cut.h"
#include "scalerData.h"

namespace cut_util {
   int LoadCuts(const char *inpath,std::vector<cut_t> &data);
   int ApplyCut(std::string var,double lo,double hi,std::vector<scalerData_t> in,std::vector<scalerData_t> &out); 
   int ApplyCuts(double cutLo,double cutHi,std::vector<double> x,std::vector<double> y,std::vector<double> &out); 
   int ApplyCuts(std::vector<cut_t> cutList,std::vector<scalerData_t> in,std::vector<scalerData_t> &out); 
   int GetStatsWithCuts(std::vector<double> x,std::vector<double> y,
                        double cutLo,double cutHi,double &mean,double &stdev); 

   bool PassCut(cut_t cut,scalerData_t event); 
}

#endif 
