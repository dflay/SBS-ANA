#ifndef BCM_PLOTTER_H
#define BCM_PLOTTER_H

// a class that can produce plots of the BCM variables 
// as a function of event number or time in the run 

#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>  

#include "TString.h"
#include "TFile.h"
#include "TChain.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TGraph.h"
#include "TGraphErrors.h"

class BCMPlotter {

   private:
      int fNEntries; 
      bool fIsDebug;
      std::vector<std::string> fVarName;   
      TTree *fTreeLeft,*fTreeSBS; 

   public: 
      BCMPlotter(const char *filePath="NONE");
      ~BCMPlotter(); 

      void SetDebug(bool v=true)  { fIsDebug = v; } 

      void LoadFile(const char *filePath);

      TH1F * GetTH1F(const char *h_name,const char *h_range,
                     const char *arm,const char *var_name,const char *type);
 
      TH2F * GetTH2F(const char *h_name,const char *h_range,
                     const char *arm,const char *xAxis,const char *yAxis,const char *type); 

}; 

#endif 
