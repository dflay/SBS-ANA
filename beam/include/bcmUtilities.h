#ifndef UTIL_BCM_H
#define UTIL_BCM_H 

// a namespace for helpful functions useful in 
// BCM data analysis 

#include <cstdlib>
#include <iostream>
#include <vector> 

#include "cut.h"
#include "codaRun.h"
#include "calibCoeff.h"
#include "producedVariable.h"
#include "scalerData.h"

namespace bcm_util {
   int GetData(std::string var,std::vector<producedVariable_t> data,std::vector<double> &x,std::vector<double> &dx); 
   int Print(producedVariable_t data); 
   int WriteToFile(const char *outpath,std::vector<producedVariable_t> data); 
   int WriteToFile_cc(const char *outpath,std::vector<calibCoeff_t> data); 
   int LoadProducedVariables(const char *inpath,std::vector<producedVariable_t> &data);
   int LoadConfigPaths(const char *inpath,std::vector<std::string> &label,std::vector<std::string> &path); 
   int LoadRuns(const char *inpath,TString &prefix,std::vector<codaRun_t> &runList); 
   int LoadCuts(const char *inpath,std::vector<cut_t> &data);
   int ApplyCuts(double cutLo,double cutHi,std::vector<double> x,std::vector<double> y,std::vector<double> &out); 
   int GetStatsWithCuts(std::vector<double> x,std::vector<double> y,
                        double cutLo,double cutHi,double &mean,double &stdev); 
   int SubtractBaseline(std::vector<producedVariable_t> on,std::vector<producedVariable_t> off, 
                        std::vector<producedVariable_t> &diff,bool isDebug=false);

   TGraphErrors *GetStatsByRun(std::string var,std::vector<scalerData_t> data);
}

#endif 
