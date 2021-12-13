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
      bool fIsDebug,fCalculateCurrent; 
      int fEvtCntrLeft,fEvtCntrSBS,fEvtCntrEPICS;
      double fLastTimeLeft,fLastTimeSBS; 

      std::vector<calibCoeff_t> fCC;  // calibration coefficients to convert rate to current [uA]  

      std::vector<scalerData_t> fLeft,fSBS;
      std::vector<epicsData_t> fEPICS;

      bool IsBad(double v);  

   public: 
      BCMManager(const char *filePath="NONE",bool isDebug=false,bool calculateCurrent=false,const char *ccFilePath="NONE");
      ~BCMManager(); 

      void SetDebug(bool v=true)         { fIsDebug          = v; }
      void CalculateCurrent(bool v=true) { fCalculateCurrent = v; }  

      void Clear(); 
      void LoadFile(const char *filePath,int runNumber=0);
      void LoadDataFromTree(const char *filePath,const char *treeName,int runNumber=0);
      void LoadEPICSDataFromTree(const char *filePath,int runNumber=0);

      int  LoadCalibrationCoefficients(const char *filePath); 

      void Print(const char *arm);
      void PrintScaler(scalerData_t data); 
      void PrintEPICS(epicsData_t data); 

      int GetCalibrationCoeff(std::string dev,std::vector<double> &v,std::vector<double> &dv); 
      int ApplyCalibrationCoeff(scalerData_t &data); 

      int GetVector(const char *arm,const char *var,std::vector<double> &v); 
      int GetVector_scaler(const char *arm,std::vector<scalerData_t> &data); 
      int GetVector_epics(std::vector<epicsData_t> &data); 

      TH1F * GetTH1F(const char *arm,const char *var_name,int NBin,double min,double max);

      TH2F * GetTH2F(const char *arm,const char *xAxis,const char *yAxis,
	             int NXBin,double xMin,double xMax,int NYBin,double yMin,double yMax);  

      TGraph * GetTGraph(const char *arm,const char *xAxis,const char *yAxis);  

}; 

#endif 
