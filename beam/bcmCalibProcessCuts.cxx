// apply cuts to BCM data
// - input: run list and cut list
// - output: mean and stdev of results of defined cuts   

#include <cstdlib>
#include <iostream>

#include "TTree.h"
#include "TFile.h"
#include "TH1F.h"
#include "TStyle.h"
#include "TPad.h"
#include "TLine.h"

#include "./include/bcm.h"
#include "./include/codaRun.h"
#include "./src/CSVManager.cxx"
#include "./src/BCMManager.cxx"
#include "./src/Utilities.cxx"
#include "./src/JSONManager.cxx"
#include "./src/Graph.cxx"
#include "./src/bcmUtilities.cxx"
#include "./src/cutUtilities.cxx"

int GetStats(std::string var,std::vector<scalerData_t> data,std::vector<double> &out); 

int bcmCalibProcessCuts(const char *confPath,const char *bcmName){
 
   int rc=0;

   // read input configuration file 
   JSONManager *jmgr = new JSONManager(confPath);
   std::string prefix   = jmgr->GetValueFromKey_str("ROOTfile-path");
   std::string run_path = jmgr->GetValueFromKey_str("run-path");
   std::string cut_path = jmgr->GetValueFromKey_str("cut-path");
   std::string out_dir  = jmgr->GetValueFromKey_str("out-dir");
   std::string bcmCalibPath = jmgr->GetValueFromKey_str("bcm-cc-path");
   // std::string devName  = jmgr->GetValueFromKey_str("dev"); 
   delete jmgr; 

   std::string devName = bcmName; 
 
   std::cout << "=================================================" << std::endl;
   std::cout << "Processing cuts for variable: " << devName << std::endl; 

   BCMManager *mgr = new BCMManager("NONE",bcmCalibPath.c_str(),false);

   std::vector<codaRun_t> runList;
   rc = util_df::LoadRunList(run_path.c_str(),prefix.c_str(),runList);
   if(rc!=0) return 1;

   util_df::LoadBCMData(runList,mgr);
   if(rc!=0) return 1;

   std::vector<cut_t> cutList; 
   rc = bcm_util::LoadCuts(cut_path.c_str(),cutList); 
   if(rc!=0) return 1; 
   
   std::string theVar;
   std::vector<double> out; 

   // vectors to store results
   std::vector<std::string> DEV,BEAM_STATE; 
   std::vector<int> GRP; 
   std::vector<double> TIME,MU,SIG;
   // for epics vars  
   std::vector<std::string> EDEV,EBEAM_STATE; 
   std::vector<int> EGRP; 
   std::vector<double> EMU,ESIG; 

   std::vector<scalerData_t> raw,data; 
   mgr->GetVector_scaler("sbs",raw); 

   char var[200]; 

   // let's do cuts on each variable defined 
   const int NC = cutList.size(); 
   for(int i=0;i<NC;i++){
      // define variable and get a vector of all data  
      if(cutList[i].arm.compare("E")==0){
	 // EPICS variable 
	 sprintf(var,"%s",cutList[i].dev.c_str()); 
      }else{
	 sprintf(var,"%s.rate",devName.c_str()); 
      }
      theVar = var; 
      // apply the cut 
      cut_util::ApplyCut(cutList[i],raw,data); 
      // compute stats on the cut data for the variable
      GetStats(theVar,data,out);
      // save results
      DEV.push_back( devName );  
      BEAM_STATE.push_back( cutList[i].beam_state );  
      GRP.push_back( cutList[i].group );  
      TIME.push_back(out[0]);  
      MU.push_back(out[1]);  
      SIG.push_back(out[2]); 
      // set up for next cut 
      data.clear();
      out.clear();
   }

   // make output directory
   util_df::MakeDirectory(out_dir.c_str());  

   char outpath[200];
   sprintf(outpath,"%s/%s.csv",out_dir.c_str(),devName.c_str());

   // write results to file
   const int NROW = DEV.size(); 
   const int NCOL = 5; 
   CSVManager *csv = new CSVManager(); 
   csv->InitTable(NROW,NCOL);
   csv->SetColumn_str(0,DEV); 
   csv->SetColumn_str(1,BEAM_STATE); 
   csv->SetColumn<int>(2,GRP); 
   csv->SetColumn<double>(3,MU); 
   csv->SetColumn<double>(4,SIG);
   csv->SetHeader("dev,beam_state,group,mean,stdev");
   csv->WriteFile(outpath); 
   delete csv; 
 
   return 0;
}
//______________________________________________________________________________
int GetStats(std::string var,std::vector<scalerData_t> data,std::vector<double> &out){
   // we want the time stamp, and mean BCM values for variable named var 
   
   const int N = data.size();
   std::vector<double> time,v; 
   for(int i=0;i<N;i++){
      time.push_back(data[i].time); 
      v.push_back( data[i].getValue(var) ); 
   }  

   // get values
   double theTime = time[0]; // note: using first time stamp! 
   double mean    = math_df::GetMean<double>(v);   
   double stdev   = math_df::GetStandardDeviation<double>(v);  

   // store in output vector
   out.push_back(theTime); 
   out.push_back(mean); 
   out.push_back(stdev); 
  
   return 0;
}
