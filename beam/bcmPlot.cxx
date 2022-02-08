// Plot all BCM data  

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

int bcmPlot(const char *confPath){

   // settings 
   bool logScale = false;

   gStyle->SetOptStat(0);

   int rc=0;

   // read input configuration file 
   JSONManager *jmgr = new JSONManager(confPath);
   std::string prefix       = jmgr->GetValueFromKey_str("ROOTfile-path");
   std::string runPath      = jmgr->GetValueFromKey_str("run-path");
   std::string bcmCalibPath = jmgr->GetValueFromKey_str("bcm-cc-path");

   BCMManager *mgr = new BCMManager("NONE",bcmCalibPath.c_str(),false);

   std::vector<codaRun_t> runList;
   rc = util_df::LoadRunList(runPath.c_str(),prefix.c_str(),runList);
   if(rc!=0) return 1;

   util_df::LoadBCMData(runList,mgr);
   if(rc!=0) return 1;

   const int N = 7; 
   TString varName[N] = {"unser.rate","u1.rate","unew.rate","dnew.rate","d1.rate","d3.rate","d10.rate"};
   TString xAxis      = Form("event"); 

   // create graphs  
   TGraph **g  = new TGraph*[N];
   TCanvas **c = new TCanvas*[N]; 

   for(int i=0;i<N;i++){
      g[i] = mgr->GetTGraph("sbs",xAxis.Data(),varName[i]);
      graph_df::SetParameters(g[i],20,kBlack);
   }

   TGraph *gEPICSCurrent = mgr->GetTGraph("E","event","IBC1H04CRCUR2"); 
   graph_df::SetParameters(gEPICSCurrent,20,kBlack); 

   TString cName,cTitle;
  
   for(int i=0;i<N;i++){
      cName  = Form("c%d",i);
      cTitle = Form("%s",varName[i].Data());
      c[i]   = new TCanvas(cName,cTitle,1200,800);
      c[i]->cd(); 
      g[i]->Draw("ap");
      graph_df::SetLabels(g[i],varName[i].Data(),xAxis.Data(),varName[i].Data());
      g[i]->Draw("ap");
      c[i]->Update(); 
   } 

   // EPICS plots

   TCanvas *ce = new TCanvas("ce","EPICS Beam Current",1200,800); 

   TString Title      = Form("Beam Current");
   TString xAxisTitle = Form("event");
   TString yAxisTitle = Form("Beam Current [#muA]");

   ce->cd();
   gEPICSCurrent->Draw("ap");
   graph_df::SetLabels(gEPICSCurrent,Title,xAxisTitle,yAxisTitle); 
   gEPICSCurrent->Draw("ap");
   ce->Update();

   return 0;
}
