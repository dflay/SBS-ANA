// Plot all BCM data  

#include <cstdlib>
#include <iostream>

#include "TTree.h"
#include "TFile.h"
#include "TH1F.h"
#include "TStyle.h"
#include "TPad.h"
#include "TLine.h"

#include "./include/codaRun.h"
#include "./src/BCMPlotter.cxx"
#include "./src/bcmUtilities.cxx"
#include "./src/Graph.cxx"

int bcmPlot(){

   // settings 
   bool logScale   = false;

   gStyle->SetOptStat(0);

   int rc=0;

   TString prefix;
   std::vector<codaRun_t> runList;  
   rc = bcm_util::LoadRuns("./input/run-list.csv",prefix,runList);
   if(rc!=0) return 1; 

   BCMPlotter *myPlotter = new BCMPlotter();
   // myPlotter->SetDebug();
   myPlotter->EnableEPICS(); 

   TString filePath;  
   const int NR = runList.size();  
   for(int i=0;i<NR;i++){ 
      filePath = Form("%s/gmn_replayed-beam_%d_stream%d_seg%d_%d.root",
                      prefix.Data(),runList[i].runNumber,runList[i].stream,runList[i].segmentBegin,runList[i].segmentEnd);
      myPlotter->LoadFile(filePath);
   } 

   const int N = 7; 
   TString varName[N] = {"u1","unew","unser","dnew","d1","d3","d10"};

   // create and TGraphs 

   TGraph **g = new TGraph*[N]; 

   for(int i=0;i<N;i++){
      g[i] = myPlotter->GetTGraph("sbs","time",varName[i],"rate");
      graph_df::SetParameters(g[i],20,kBlack); 
   }

   TGraph *gEPICSCurrent = myPlotter->GetTGraph("E","time","IBC1H04CRCUR2",""); 
   gEPICSCurrent->SetMarkerStyle(20);

   TCanvas *c1a = new TCanvas("c1a","BCM Check",1200,800);
   c1a->Divide(2,2);

   TCanvas *c1b = new TCanvas("c1b","BCM Check",1200,800);
   c1b->Divide(2,2);

   TString Title,yAxisTitle;
   TString xAxisTitle = Form("Time [sec]"); 

   for(int i=0;i<N/2;i++){
      c1a->cd(i+1);
      Title      = Form("%s"     ,varName[i].Data());
      yAxisTitle = Form("%s [Hz]",varName[i].Data());
      g[i]->Draw("alp");
      graph_df::SetLabels(g[i],Title,xAxisTitle,yAxisTitle); 
      g[i]->Draw("alp");
      c1a->Update();
      // next canvas 
      c1b->cd(i+1);
      Title      = Form("%s"     ,varName[i+3].Data());
      yAxisTitle = Form("%s [Hz]",varName[i+3].Data());
      g[i+3]->Draw("alp");
      graph_df::SetLabels(g[i+3],Title,xAxisTitle,yAxisTitle); 
      g[i+3]->Draw("alp");
      c1b->Update();
   }

   // last one 
   Title      = Form("%s"     ,varName[6].Data());
   yAxisTitle = Form("%s [Hz]",varName[6].Data());
   c1b->cd(4); 
   g[6]->Draw("alp");
   graph_df::SetLabels(g[6],Title,xAxisTitle,yAxisTitle); 
   g[6]->Draw("alp");
   c1b->Update();

   // EPICS plots

   TCanvas *c3 = new TCanvas("c3","EPICS Beam Current",1200,800); 

   Title      = Form("IBC1H04CRCUR2"       );
   yAxisTitle = Form("IBC1H04CRCUR2 [#muA]");

   c3->cd();
   gEPICSCurrent->Draw("alp");
   graph_df::SetLabels(gEPICSCurrent,Title,xAxisTitle,yAxisTitle); 
   gEPICSCurrent->Draw("alp");
   c3->Update();

   return 0;
}
