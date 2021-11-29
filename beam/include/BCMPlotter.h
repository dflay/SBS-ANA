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
      TChain *fChainLeft,*fChainSBS;
      TString fLeftVarTime_num,fLeftVarTime_den;
      TString fSBSVarTime_num,fSBSVarTime_den;

      double fu1r_left,fu1c_left;
      double funewr_left,funewc_left;
      double fd1r_left,fd1c_left;
      double fd3r_left,fd3c_left;
      double fd10r_left,fd10c_left;
      double fdnewr_left,fdnewc_left;
      double funserr_left,funserc_left;
      double ftime_left_num,ftime_left_den;

      double fu1r_sbs,fu1c_sbs;
      double funewr_sbs,funewc_sbs;
      double fd1r_sbs,fd1c_sbs;
      double fd3r_sbs,fd3c_sbs;
      double fd10r_sbs,fd10c_sbs;
      double fdnewr_sbs,fdnewc_sbs;
      double funserr_sbs,funserc_sbs;
      double ftime_sbs_num,ftime_sbs_den;

      int CheckVariable(const char *arm,const char *var);
      int SetBranchAddresses(); 

   public: 
      BCMPlotter(const char *filePath="NONE");
      ~BCMPlotter(); 

      void SetDebug(bool v=true)  { fIsDebug = v; } 
      void LoadFile(const char *filePath);
      void SetTimeVariable(const char *arm,const char *var); 

      int GetVector(const char *arm,const char *var,std::vector<double> &v); 

      TH1F * GetTH1F(const char *h_name,const char *h_range,
                     const char *arm,const char *var_name,const char *type);
 
      TH2F * GetTH2F(const char *h_name,const char *h_range,
                     const char *arm,const char *xAxis,const char *yAxis,const char *type);

      TGraph * GetTGraph(const char *arm,const char *xAxis,const char *yAxis,const char *type);  

}; 

#endif 
