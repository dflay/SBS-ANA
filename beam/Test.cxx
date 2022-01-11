// Plot all BCM data  

#include <cstdlib>
#include <iostream>

#include "TTree.h"
#include "TFile.h"
#include "TH1F.h"
#include "TStyle.h"
#include "TPad.h"
#include "TLine.h"

#include "./include/scalerData.h"
#include "./include/codaRun.h"
#include "./src/Graph.cxx"
#include "./src/bcmUtilities.cxx"
#include "./src/JSONManager.cxx"

int Test(){

   // scalerData_t data; 
   // data.unserCurrent = 15.; 
   // std::cout << data.Print("rate") << std::endl;

   JSONManager *jmgr = new JSONManager("./input/json/test.json");
   jmgr->Print(); 

   std::string cutFile = jmgr->GetValueFromKey_str("cut-list"); 
   std::cout << cutFile << std::endl; 

   std::vector<double> cc; 

   int NCC = jmgr->GetValueFromKey<int>("ncc"); 
   jmgr->GetVectorFromKey<double>("calib-coeffs",NCC,cc); 

   for(int i=0;i<NCC;i++){
      std::cout << cc[i] << std::endl;
   }

   // // settings 
   // bool logScale   = false;

   // gStyle->SetOptStat(0);

   // int rc=0;

   // TString prefix;
   // std::vector<codaRun_t> runList;  
   // rc = bcm_util::LoadRuns(runPath,prefix,runList);
   // if(rc!=0) return 1; 

   // BCMManager *mgr = new BCMManager();
   // mgr->SetDebug();
   // mgr->EnableEPICS();

   // TString filePath;  
   // const int NR = runList.size();  
   // for(int i=0;i<NR;i++){ 
   //    filePath = Form("%s/gmn_replayed-beam_%d_stream%d_seg%d_%d.root",
   //                    prefix.Data(),runList[i].runNumber,runList[i].stream,runList[i].segmentBegin,runList[i].segmentEnd);
   //    mgr->LoadFile(filePath,runList[i].runNumber);
   // }

   // mgr->Print("Left"); 

   return 0;
}
