#ifndef UTIL_BCM_H
#define UTIL_BCM_H 

// a namespace for helpful functions useful in 
// BCM data analysis 

#include <cstdlib>
#include <iostream>
#include <vector> 

#include "cut.h"

namespace bcm_util { 
   int LoadCuts(const char *inpath,std::vector<cut_t> &data);

}

#endif 
