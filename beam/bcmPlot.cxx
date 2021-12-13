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
#include "./src/Graph.cxx"
#include "./src/bcmUtilities.cxx"

int bcmPlot(const char *runPath){

   // settings 
   bool logScale   = false;

   gStyle->SetOptStat(0);

   int rc=0;

   TString prefix;
   std::vector<codaRun_t> runList;  
   rc = bcm_util::LoadRuns(runPath,prefix,runList);
   if(rc!=0) return 1; 

   BCMManager *mgr = new BCMManager("NONE",false,true,"./input/calib-coeff_12-01-21.csv");

   TString filePath;  
   const int NR = runList.size();  
   for(int i=0;i<NR;i++){ 
      filePath = Form("%s/gmn_replayed-beam_%d_stream%d_seg%d_%d.root",
                      prefix.Data(),runList[i].runNumber,runList[i].stream,runList[i].segmentBegin,runList[i].segmentEnd);
      mgr->LoadFile(filePath,runList[i].runNumber);
   }

   const int N = 7; 
   TString varName[N] = {"u1.rate","unew.rate","unser.rate","dnew.rate","d1.rate","d3.rate","d10.rate"};
   TString xAxis      = Form("event"); 

   // create graphs and histograms 

   TGraph **g = new TGraph*[N];
   TH1F **h   = new TH1F*[N]; 

   int NBin = 100; 
   // histo bounds  u1  unew    unser  dnew d1     d3    d10  
   double min[N] = {0    ,0    ,700E+3,0    ,0    ,0    ,0    };
   double max[N] = {10E+3,50E+3,900E+3,50E+3,10E+3,10E+3,10E+3};

   for(int i=0;i<N;i++){
      g[i] = mgr->GetTGraph("sbs",xAxis.Data(),varName[i]);
      graph_df::SetParameters(g[i],20,kBlack);
      h[i] = mgr->GetTH1F("sbs",varName[i].Data(),NBin,min[i],max[i]); 
   }

   TGraph *gEPICSCurrent = mgr->GetTGraph("E","event","IBC1H04CRCUR2"); 
   graph_df::SetParameters(gEPICSCurrent,20,kBlack);  

   TGraph *gUnserCurrent = mgr->GetTGraph("sbs","event","unser.current");
   graph_df::SetParameters(gUnserCurrent,20,kRed);  

   TMultiGraph *mgc = new TMultiGraph();
   mgc->Add(gEPICSCurrent,"lp"); 
   mgc->Add(gUnserCurrent,"lp"); 

   TLegend *L = new TLegend(0.6,0.6,0.8,0.8);
   L->AddEntry(gEPICSCurrent,"EPICS Current (IBC1H04CRCUR2)","p"); 
   L->AddEntry(gUnserCurrent,"Unser Current","p"); 

   TCanvas *c1a = new TCanvas("c1a","BCM Check",1200,800);
   c1a->Divide(2,2);

   TCanvas *c1b = new TCanvas("c1b","BCM Check",1200,800);
   c1b->Divide(2,2);

   TString Title,xAxisTitle,yAxisTitle;
   if(xAxis=="time"){
      xAxisTitle = Form("%s [sec]",xAxis.Data());
   }else{
      xAxisTitle = Form("%s",xAxis.Data());
   }

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

   // histograms
   TCanvas *c2a = new TCanvas("c2a","BCM Check [Histograms]",1200,800);
   c2a->Divide(2,2);

   TCanvas *c2b = new TCanvas("c2b","BCM Check [Histograms]",1200,800);
   c2b->Divide(2,2);

   for(int i=0;i<N/2;i++){
      c2a->cd(i+1);
      h[i]->Draw("");
      c2a->Update();
      // next canvas 
      c2b->cd(i+1);
      h[i+3]->Draw("");
      c2b->Update();
   }

   // last one 
   c2b->cd(4); 
   h[6]->Draw("");
   c2b->Update();

   // EPICS plots

   TCanvas *c3 = new TCanvas("c3","EPICS Beam Current",1200,800); 

   Title      = Form("Beam Current"       );
   yAxisTitle = Form("Beam Current [#muA]");

   c3->cd();
   mgc->Draw("a");
   graph_df::SetLabels(mgc,Title,xAxisTitle,yAxisTitle); 
   mgc->Draw("a");
   L->Draw("same"); 
   c3->Update();

   return 0;
}
