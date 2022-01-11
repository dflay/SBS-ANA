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

#include "./include/cut.h"
#include "./include/codaRun.h"
#include "./src/bcmUtilities.cxx"
#include "./src/JSONManager.cxx"

int bcmProcessCuts(const char *confPath){
 
   int rc=0;

   // read input configuration file 
   JSONManager *jmgr = new JSONManager(confPath);
   std::string prefix   = jmgr->GetValueFromKey_str("ROOTfile-path");
   std::string run_path = jmgr->GetValueFromKey_str("run-path");
   std::string cut_path = jmgr->GetValueFromKey_str("cut-path");
   std::string out_path = jmgr->GetValueFromKey_str("out-path");
   delete jmgr; 
   
   std::vector<codaRun_t> runList;
   rc = bcm_util::LoadRuns(run_path.c_str(),runList);
   if(rc!=0) return 1; 
   
   BCMManager *mgr = new BCMManager();

   TString filePath;  
   const int NR = runList.size();  
   for(int i=0;i<NR;i++){ 
      std::cout << Form("Loading run %d",runList[i].runNumber) << std::endl;
      filePath = Form("%s/gmn_replayed-beam_%d_stream%d_seg%d_%d.root",
                      prefix.c_str(),runList[i].runNumber,runList[i].stream,runList[i].segmentBegin,runList[i].segmentEnd);
      mgr->LoadFile(filePath);
   } 

   std::vector<cut_t> cutList; 
   rc = bcm_util::LoadCuts(cut_path.c_str(),cutList); 
   if(rc!=0) return 1; 
   
   TString theVar; 
   double mean=0,stdev=0;
   std::vector<double> v,time; 

   // vectors to store results
   std::vector<std::string> DEV,BEAM_STATE; 
   std::vector<int> GRP; 
   std::vector<double> MU,SIG;
   // for epics vars  
   std::vector<std::string> EDEV,EBEAM_STATE; 
   std::vector<int> EGRP; 
   std::vector<double> EMU,ESIG; 

   // let's do cuts on each variable defined 
   const int NC = cutList.size(); 
   for(int i=0;i<NC;i++){
      // define variable and get a vector of all data  
      if(cutList[i].arm.compare("E")==0){
	 // EPICS variable 
	 theVar = Form("%s",cutList[i].dev.c_str());
      }else{
	 theVar = Form("%s.rate",cutList[i].dev.c_str());
      }
      mgr->GetVector(cutList[i].arm.c_str(),"time",time);
      mgr->GetVector(cutList[i].arm.c_str(),theVar.Data(),v);
      // compute stats with cuts 
      bcm_util::GetStatsWithCuts(time,v,cutList[i].low,cutList[i].high,mean,stdev);
      std::cout << Form("[Cuts applied: cut lo = %.3lf, cut hi = %.3lf, group: %d]: %s mean = %.3lf, stdev = %.3lf",
                        cutList[i].low,cutList[i].high,cutList[i].group,theVar.Data(),mean,stdev) << std::endl; 
      // save results
      if(cutList[i].arm.compare("E")==0){
	 EDEV.push_back( cutList[i].dev );  
	 EBEAM_STATE.push_back( cutList[i].beam_state );  
	 EGRP.push_back( cutList[i].group );  
	 EMU.push_back(mean);  
	 ESIG.push_back(stdev); 
      }else{ 
	 DEV.push_back( cutList[i].dev );  
	 BEAM_STATE.push_back( cutList[i].beam_state );  
	 GRP.push_back( cutList[i].group );  
	 MU.push_back(mean);  
	 SIG.push_back(stdev); 
      } 
      // set up for next cut 
      v.clear();
      time.clear();
   }

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
   csv->WriteFile(out_path.c_str()); 
   delete csv; 
 
   return 0;
}
