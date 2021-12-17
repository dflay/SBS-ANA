// Plot all BCM data vs run number  

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

int bcmCalibByRun(const char *runPath){

   // settings 
   bool logScale   = false;

   gStyle->SetOptStat(0);

   int rc=0;

   TString prefix;
   std::vector<codaRun_t> runList;  
   rc = bcm_util::LoadRuns(runPath,prefix,runList);
   if(rc!=0) return 1; 

   BCMManager *mgr = new BCMManager("NONE",false,"./input/calib-coeff_12-01-21.csv");

   TString filePath;  
   const int NR = runList.size();  
   for(int i=0;i<NR;i++){ 
      filePath = Form("%s/gmn_replayed-beam_%d_stream%d_seg%d_%d.root",
                      prefix.Data(),runList[i].runNumber,runList[i].stream,runList[i].segmentBegin,runList[i].segmentEnd);
      mgr->LoadFile(filePath,runList[i].runNumber);
   }

   // do stats by run number 
   std::vector<scalerData_t> data;
   mgr->GetVector_scaler("sbs",data);   

   // apply cuts
   // ...  

   const int N = 7; 
   TString var[N] = {"unser.rate","u1.rate","unew.rate","d1.rate","d3.rate","d10.rate","dnew.rate"};

   TGraphErrors **g  = new TGraphErrors*[N]; 
   TGraphErrors **gc = new TGraphErrors*[N]; 

   for(int i=0;i<N;i++){
      g[i]  = bcm_util::GetTGraphErrors_byRun(var[i].Data(),data);
      graph_df::SetParameters(g[i],20,kBlack); 
      gc[i] = bcm_util::GetTGraphErrors_byRunByUnserCurrent(var[i].Data(),data);
      graph_df::SetParameters(gc[i],20,kBlack); 
   }

   TGraphErrors *gUnserCurrent_byRun = bcm_util::GetTGraphErrors_byRun("unser.current",data);
   graph_df::SetParameters(gUnserCurrent_byRun,20,kBlack);

   TGraph *gUnserCurrent_byEvent = mgr->GetTGraph("sbs","event","unser.current");  
   graph_df::SetParameters(gUnserCurrent_byEvent,20,kBlack);

   TString Title,yAxisTitle;
   TString xAxisTitle = Form("Run Number");

   TCanvas *c1a = new TCanvas("c1a","BCM Check by Run",1200,800);
   c1a->Divide(2,2);

   TCanvas *c1b = new TCanvas("c1b","BCM Check by Run",1200,800);
   c1b->Divide(2,2); 

   for(int i=0;i<N/2;i++){
      c1a->cd(i+1);
      Title      = Form("%s"     ,var[i].Data());
      yAxisTitle = Form("%s [Hz]",var[i].Data());
      g[i]->Draw("alp");
      graph_df::SetLabels(g[i],Title,xAxisTitle,yAxisTitle);
      g[i]->Draw("alp");
      c1a->Update();
      // next canvas 
      c1b->cd(i+1);
      Title      = Form("%s"     ,var[i+3].Data());
      yAxisTitle = Form("%s [Hz]",var[i+3].Data());
      g[i+3]->Draw("alp");
      graph_df::SetLabels(g[i+3],Title,xAxisTitle,yAxisTitle);
      g[i+3]->Draw("alp");
      c1b->Update();
   }

   // last one 
   Title      = Form("%s"     ,var[6].Data());
   yAxisTitle = Form("%s [Hz]",var[6].Data());
   c1b->cd(4);
   g[6]->Draw("alp");
   graph_df::SetLabels(g[6],Title,xAxisTitle,yAxisTitle);
   g[6]->Draw("alp");
   c1b->Update();

   TCanvas *c2a = new TCanvas("c2a","BCM Calibration by Run",1200,800);
   c2a->Divide(2,2);

   TCanvas *c2b = new TCanvas("c2b","BCM Calibration by Run",1200,800);
   c2b->Divide(2,2); 

   xAxisTitle = Form("Unser Current [#muA]"); 

   for(int i=0;i<N/2;i++){
      c2a->cd(i+1);
      Title      = Form("%s"     ,var[i].Data());
      yAxisTitle = Form("%s [Hz]",var[i].Data());
      gc[i]->Draw("ap");
      graph_df::SetLabels(gc[i],Title,xAxisTitle,yAxisTitle);
      gc[i]->Draw("ap");
      c2a->Update();
      // next canvas 
      c2b->cd(i+1);
      Title      = Form("%s"     ,var[i+3].Data());
      yAxisTitle = Form("%s [Hz]",var[i+3].Data());
      gc[i+3]->Draw("ap");
      graph_df::SetLabels(gc[i+3],Title,xAxisTitle,yAxisTitle);
      gc[i+3]->Draw("ap");
      c2b->Update();
   }

   // last one 
   Title      = Form("%s"     ,var[6].Data());
   yAxisTitle = Form("%s [Hz]",var[6].Data());
   c2b->cd(4);
   gc[6]->Draw("ap");
   graph_df::SetLabels(gc[6],Title,xAxisTitle,yAxisTitle);
   gc[6]->Draw("ap");
   c1b->Update();

   TCanvas *c3 = new TCanvas("c3","Unser Current",1200,800);
   c3->Divide(1,2);
 
   c3->cd(1);
   gUnserCurrent_byEvent->Draw("alp");
   graph_df::SetLabels(gUnserCurrent_byEvent,"Unser Current","Event","Unser Current [#muA]");
   gUnserCurrent_byEvent->Draw("alp");
   c3->Update(); 

   c3->cd(2);
   gUnserCurrent_byRun->Draw("ap");
   graph_df::SetLabels(gUnserCurrent_byRun,"Unser Current","Run Number","Unser Current [#muA]");
   gUnserCurrent_byRun->Draw("ap");
   c3->Update(); 

   return 0;
}
