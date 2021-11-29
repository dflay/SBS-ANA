#ifndef UTIL_BCM_H
#define UTIL_BCM_H 

// a namespace for helpful functions useful in 
// BCM data analysis 

#include <cstdlib>
#include <iostream>
#include <vector> 

#include "cut.h"
#include "producedVariable.h"

namespace bcm_util {
   int LoadProducedVariables(const char *inpath,std::vector<producedVariable_t> &data); 
   int LoadConfigPaths(const char *inpath,std::vector<std::string> &label,std::vector<std::string> &path); 
   int LoadRuns(const char *inpath,TString &prefix,std::vector<int> &run); 
   int LoadCuts(const char *inpath,std::vector<cut_t> &data);
   void SubtractBaseline(std::vector<producedVariable_t> on,std::vector<producedVariable_t> off, 
                         std::vector<producedVariable_t> &diff,bool isDebug=false);
}

#endif 
