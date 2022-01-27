// Plot all BCM data  

#include <cstdlib>
#include <iostream>

#include "TTree.h"
#include "TFile.h"
#include "TH1F.h"
#include "TStyle.h"
#include "TPad.h"
#include "TLine.h"

#include "./include/rootData.h"
#include "./include/scalerData.h"
#include "./include/codaRun.h"
#include "./src/Event.cxx"
#include "./src/Graph.cxx"
#include "./src/JSONManager.cxx"
#include "./src/Utilities.cxx"
#include "./src/bcmUtilities.cxx"
#include "./src/ROOTFileManager.cxx"

int Test(){

   // int NEV = 20; 
   // int N = 3;
   // int M = 2;
   // double iv=0,iw=0;
   // std::vector<CSVManager *> data; 
   // for(int i=0;i<N;i++){
   //    CSVManager *csv = new CSVManager();
   //    csv->InitTable(NEV,M); 
   //    for(int j=0;j<NEV;j++){
   //       iv = i*3 + j*2;
   //       iw = i*5 + j*7;  
   //       csv->SetElement<double>(j,0,iv); 
   //       csv->SetElement<double>(j,1,iw); 
   //    } 
   //    data.push_back(csv);
   //    data[i]->Print();
   //    // v.clear();
   // }
   // for(int i=0;i<N;i++) delete data[i]; 
   // data.clear(); 

   std::string prefix = "/lustre19/expphy/volatile/halla/sbs/flay/GMnAnalysis/rootfiles";
   util::rootData_t rfData; 
   rfData.fileName      = prefix + "/gmn_replayed-beam_13297_stream0_seg0_2.root";
   rfData.structurePath = "./input/format/beam.csv";  
 
   std::cout << "TRYING FILE: " << rfData.fileName << std::endl; 
 
   util::ROOTFileManager *rfMgr = new util::ROOTFileManager();
   rfMgr->LoadFile(rfData.fileName.Data(),rfData.structurePath.Data());
   rfMgr->Print();

   std::vector<double> unserRate; 
   rfMgr->GetVector<double>("TSsbs","sbs.bcm.unser.rate",unserRate); 

   const int N = unserRate.size();
   for(int i=0;i<N;i++) std::cout << Form("%.3lf",unserRate[i]) << std::endl;
 
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
