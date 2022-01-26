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
#include "./src/JSONManager.cxx"
#include "./src/Utilities.cxx"
#include "./src/bcmUtilities.cxx"
#include "./src/ROOTFileManager.cxx"

int Test(){

   // int N = 3;
   // int M = 10;
   // std::vector<double> v; 
   // std::vector<CSVManager *> data; 
   // for(int i=0;i<N;i++){
   //    CSVManager *csv = new CSVManager();
   //    csv->InitTable(M,1); 
   //    for(int j=0;j<M;j++) v.push_back( i*5 ); 
   //    csv->SetColumn<double>(0,v);   
   //    data.push_back(csv);
   //    data[i]->Print();
   //    v.clear();
   // }
   // for(int i=0;i<N;i++) delete data[i]; 
   // data.clear(); 

   util::ROOTFileManager *rfMgr = new util::ROOTFileManager(); 
   rfMgr->LoadFileStructure("./input/format/beam.csv");
   delete rfMgr; 

   // scalerData_t data; 
   // data.unserCurrent = 15.; 
   // std::cout << data.Print("rate") << std::endl;

   // JSONManager *jmgr = new JSONManager("./input/json/test.json");
   // jmgr->Print(); 

   // std::string cutFile = jmgr->GetValueFromKey_str("cut-list"); 
   // std::cout << cutFile << std::endl; 

   // std::vector<double> cc; 

   // int NCC = jmgr->GetValueFromKey<int>("ncc"); 
   // jmgr->GetVectorFromKey<double>("calib-coeffs",NCC,cc); 

   // for(int i=0;i<NCC;i++){
   //    std::cout << cc[i] << std::endl;
   // }

   // std::string rfPath = jmgr->GetValueFromKey_str("ROOTfile-path");
   // int run=11913;
   // std::vector<int> data; 
   // int rc = util_df::GetROOTFileMetaData(rfPath.c_str(),run,data); 

   // int stream=-1,begSeg=-1,endSeg=-1; 
   // if(rc==0){
   //    stream = data[0]; 
   //    begSeg = data[1]; 
   //    endSeg = data[2];
   //    std::cout << Form("Run %d, begSeg = %d, endSeg = %d",run,begSeg,endSeg) << std::endl;
   // }

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
