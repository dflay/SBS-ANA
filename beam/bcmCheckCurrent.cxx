// Plot all BCM data vs run number  

#include <cstdlib>
#include <iostream>

#include "TTree.h"
#include "TFile.h"
#include "TH1F.h"
#include "TStyle.h"
#include "TLine.h"

#include "./include/bcm.h"
#include "./include/codaRun.h"
#include "./src/ABA.cxx"
#include "./src/CSVManager.cxx"
#include "./src/BCMManager.cxx"
#include "./src/Utilities.cxx"
#include "./src/JSONManager.cxx"
#include "./src/Graph.cxx"
#include "./src/bcmUtilities.cxx"
#include "./src/cutUtilities.cxx"

#define MICROAMPS 1E-6

TString GetTitle(std::vector<int> run); 

int bcmCheckCurrent(const char *confPath){

   // settings 
   bool logScale = false;

   gStyle->SetOptStat(0);

   int rc=0;

   // read input configuration file 
   JSONManager *jmgr = new JSONManager(confPath);
   std::string prefix       = jmgr->GetValueFromKey_str("ROOTfile-path"); 
   std::string runPath      = jmgr->GetValueFromKey_str("run-path");  
   std::string cutPath      = jmgr->GetValueFromKey_str("cut-path"); 
   
   BCMManager *mgr = new BCMManager("NONE","NONE",false);

   std::vector<codaRun_t> runList;  
   rc = util_df::LoadRunList(runPath.c_str(),prefix.c_str(),runList);
   if(rc!=0) return 1; 

   util_df::LoadBCMData(runList,mgr); 
   if(rc!=0) return 1; 

   // do stats by run number 
   std::vector<scalerData_t> sbs,lhrs;
   mgr->GetVector_scaler("sbs" ,sbs);  
   mgr->GetVector_scaler("Left",lhrs);  

   std::vector<epicsData_t> edata; 
   mgr->GetVector_epics(edata);  

   // // load cuts 
   // std::vector<cut_t> cutList; 
   // cut_util::LoadCuts(cutPath.c_str(),cutList);
   // // apply cuts  
   // cut_util::ApplyCuts(cutList,rawData,data);

   // sort the data by run number 
   std::sort(sbs.begin() ,sbs.end() ,compareScalerData_byRun); 
   std::sort(lhrs.begin(),lhrs.end(),compareScalerData_byRun); 
   std::sort(edata.begin(),edata.end(),compareEPICSData_byRun); 

   std::vector<int> rr; 
   mgr->GetRunList(rr); 
   const int NNR = rr.size();

   const int N = 6; 
   // TString varC[N] = {"unser.current","u1.current","unew.current","d1.current","d3.current","d10.current","dnew.current"};
   // int color[N]    = {kBlack,kRed,kBlue,kGreen+2,kMagenta,kCyan+2,kOrange}; 
   TString varC[N] = {"u1.current","unew.current","d1.current","d3.current","d10.current","dnew.current"};
   int color[N]    = {kBlack,kRed,kBlue,kGreen+2,kMagenta,kCyan+2}; 

   std::string xVar    = "time"; 
   TString xAxisTitle  = Form("");
   TString canvasTitle = GetTitle(rr);
   TString Title,yAxisTitle; 

   TLegend *L = new TLegend(0.6,0.6,0.8,0.8); 
   TMultiGraph **mg = new TMultiGraph*[N]; 
   TGraph **gLeft   = new TGraph*[N]; 
   TGraph **gSBS    = new TGraph*[N]; 

   TCanvas *c1 = new TCanvas("c1","BCM Currents",1200,800);
   c1->Divide(3,2); 

   // get BCM plots 
   for(int i=0;i<N;i++){
      Title      = Form("%s",varC[i].Data());
      yAxisTitle = Form("%s [#muA]",varC[i].Data());
      gSBS[i]  = bcm_util::GetTGraph(xVar.c_str(),varC[i].Data(),sbs ); 
      gLeft[i] = bcm_util::GetTGraph(xVar.c_str(),varC[i].Data(),lhrs);
      graph_df::SetParameters(gSBS[i] ,21,kBlack);
      graph_df::SetParameters(gLeft[i],20,kRed  );
      mg[i]    = new TMultiGraph();
      mg[i]->Add(gSBS[i],"p");  
      // mg[i]->Add(gLeft[i],"p"); 
      c1->cd(i+1);
      mg[i]->Draw("a"); 
      graph_df::SetLabels(mg[i],Title,xAxisTitle,yAxisTitle); 
      graph_df::UseTimeDisplay(mg[i]);  
      mg[i]->Draw("a"); 
      c1->Update();
   }
  
   // TCanvas *c3 = new TCanvas("c3",canvasTitle,1200,800);

   // c3->cd();
   // mgc->Draw("a"); 
   // graph_df::SetLabels(mgc,Title,xAxisTitle,"Beam Current [#muA]");
   // // graph_df::SetLabelSizes(mgc,0.05,0.06);  
   // graph_df::UseTimeDisplay(mgc);  
   // mgc->Draw("a");
   // L->Draw("same"); 
   // c3->Update(); 

   delete mgr;
   delete jmgr; 

   return 0;
}
//______________________________________________________________________________
TString GetTitle(std::vector<int> run){
   TString title="";
   int first=0,last=0;
   const int N = run.size();
   if(N==0){
      std::cout << "[GetTitle]: No runs!" << std::endl;
   }else if(N==1){
      title = Form("Run %d: Beam Current",run[0]);
   }else{
      title = Form("Run %d--%d: Beam Current",run[0],run[N-1]);
   } 
   return title;
}
