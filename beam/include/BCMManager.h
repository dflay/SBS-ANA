#ifndef BCM_MANAGER_H
#define BCM_MANAGER_H

// a class that can produce plots of the BCM variables 
// as a function of event number or time in the run 

#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>  

#include "TString.h"
#include "TFile.h"
#include "TChain.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TGraph.h"
#include "TGraphErrors.h"

#include "scalerData.h"
#include "epicsData.h"

class BCMManager {

   private:
      bool fIsDebug; 
      int fEvtCntrLeft,fEvtCntrSBS,fEvtCntrEPICS;
      double fLastTimeLeft,fLastTimeSBS; 

      std::vector<scalerData_t> fLeft,fSBS;
      std::vector<epicsData_t> fEPICS;

      bool IsBad(double v);  

   public: 
      BCMManager(const char *filePath="NONE",bool isDebug=false);
      ~BCMManager(); 

      void SetDebug(bool v=true)  { fIsDebug = v; } 

      void Clear(); 
      void LoadFile(const char *filePath,int runNumber=0);
      void LoadDataFromTree(const char *filePath,const char *treeName,int runNumber=0);
      void LoadEPICSDataFromTree(const char *filePath,int runNumber=0);

      void Print(const char *arm);
      void PrintScaler(scalerData_t data); 
      void PrintEPICS(epicsData_t data); 

      int GetVector(const char *arm,const char *var,std::vector<double> &v); 

      TH1F * GetTH1F(const char *arm,const char *var_name,int NBin,double min,double max);

      TH2F * GetTH2F(const char *arm,const char *xAxis,const char *yAxis,
	             int NXBin,double xMin,double xMax,int NYBin,double yMin,double yMax);  

      TGraph * GetTGraph(const char *arm,const char *xAxis,const char *yAxis);  

}; 

#endif 
