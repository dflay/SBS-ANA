// Check the chosen cuts for the BCM data  

#include <cstdlib>
#include <iostream>

#include "TTree.h"
#include "TFile.h"
#include "TH1F.h"
#include "TStyle.h"
#include "TPad.h"
#include "TLine.h"

#include "./include/cut.h"
#include "./src/Graph.cxx"
#include "./src/BCMManager.cxx"
#include "./src/bcmUtilities.cxx"

int bcmCheckCuts(){

   // settings 
   bool logScale   = false;
   
   gStyle->SetOptStat(0);
 
   int rc=0;

   TString prefix; 
   std::vector<int> runList;
   rc = bcm_util::LoadRuns("./input/run-list.csv",prefix,runList);
   if(rc!=0) return 1; 
   
   BCMManager *mgr = new BCMManager();
   // mgr->SetDebug();
   mgr->EnableEPICS(); 

   TString filePath;  
   int startSegment = 0; 
   int endSegment   = 0; 
   const int NR = runList.size();  
   for(int i=0;i<NR;i++){ 
      filePath = Form("%s/gmn_replayed-beam_%d_stream0_seg%d_%d.root",prefix.Data(),runList[i],startSegment,endSegment);
      mgr->LoadFile(filePath);
   } 

   std::vector<cut_t> cutList; 
   rc = bcm_util::LoadCuts("./input/cut-list.csv",cutList); 
   if(rc!=0) return 1; 

   const int N = 7; 
   TString varName[N] = {"u1.rate","unew.rate","unser.rate","dnew.rate","d1.rate","d3.rate","d10.rate"};

   int NBin = 100;
   // histo bounds        u1  unew unser  dnew  d1     d3  d10
   double loBeamOff[N] = {50  ,0  ,700E+3,0    ,1E+3  ,0  ,0  };
   double hiBeamOff[N] = {150,100 ,850E+3,20E+3,1.5E+3,100,100};

   TString theVar; 
   double mean=0,stdev=0;
   std::vector<double> time,v; 

   const int NC = cutList.size();
   TLine **lo = new TLine*[NC]; 
   TLine **hi = new TLine*[NC]; 

   // let's do cuts on each variable defined 
   for(int i=0;i<NC;i++){
      // define variable and get a vector of all data  
      if(cutList[i].arm.compare("E")==0){
	 // EPICS variable 
	 theVar = Form("%s",cutList[i].dev.c_str());
      }else{
	 theVar = Form("%s.bcm.%s.rate",cutList[i].arm.c_str(),cutList[i].dev.c_str());
      }
      mgr->GetVector(cutList[i].arm.c_str(),"time",time); 
      mgr->GetVector(cutList[i].arm.c_str(),theVar.Data(),v); 
      bcm_util::GetStatsWithCuts(time,v,cutList[i].low,cutList[i].high,mean,stdev);
      std::cout << Form("[Cuts applied: cut lo = %.3lf, cut hi = %.3lf, group: %d]: %s mean = %.3lf, stdev = %.3lf",
                        cutList[i].low,cutList[i].high,cutList[i].group,theVar.Data(),mean,stdev) << std::endl; 
      // make lines we can plot 
      lo[i] = new TLine(cutList[i].low ,mean-5*stdev,cutList[i].low ,mean+5*stdev);
      lo[i]->SetLineColor(kRed); 
      hi[i] = new TLine(cutList[i].high,mean-5*stdev,cutList[i].high,mean+5*stdev);
      hi[i]->SetLineColor(kRed);
      // set up for next cut 
      v.clear();
      time.clear();
   }
 
   // create histos and TGraphs 

   TGraph **g = new TGraph*[N]; 

   for(int i=0;i<N;i++){
      g[i] = mgr->GetTGraph("sbs","time",varName[i]);
      graph_df::SetParameters(g[i],20,kBlack); 
   }

   TGraph *gEPICSCurrent = mgr->GetTGraph("E","time","IBC1H04CRCUR2"); 
   gEPICSCurrent->SetMarkerStyle(20);

   TCanvas *c1a = new TCanvas("c1a","BCM Check",1200,800);
   c1a->Divide(2,2);

   TCanvas *c1b = new TCanvas("c1b","BCM Check",1200,800);
   c1b->Divide(2,2);

   TString Title,yAxisTitle;

   for(int i=0;i<N/2;i++){
      c1a->cd(i+1);
      g[i]->Draw();
      Title      = Form("%s"     ,varName[i].Data());
      yAxisTitle = Form("%s [Hz]",varName[i].Data());
      graph_df::SetLabels(g[i+3],Title,"Time [sec]",yAxisTitle); 
      g[i]->Draw();
      lo[i]->Draw("same"); 
      hi[i]->Draw("same");
      c1a->Update();
      c1b->cd(i+1);
      g[i+3]->Draw();
      Title      = Form("%s"     ,varName[i+3].Data());
      yAxisTitle = Form("%s [Hz]",varName[i+3].Data());
      graph_df::SetLabels(g[i+3],Title,"Time [sec]",yAxisTitle); 
      g[i+3]->Draw();
      lo[i+3]->Draw("same"); 
      hi[i+3]->Draw("same");
      c1b->Update();
   }

   // last one 
   c1b->cd(4); 
   g[6]->Draw();
   Title      = Form("%s"     ,varName[6].Data());
   yAxisTitle = Form("%s [Hz]",varName[6].Data());
   graph_df::SetLabels(g[6],Title,"Time [sec]",yAxisTitle); 
   g[6]->Draw();
   lo[6]->Draw("same"); 
   hi[6]->Draw("same");
   c1b->Update();

   // EPICS plots

   TCanvas *c3 = new TCanvas("c3","EPICS Beam Current",1200,800); 

   c3->cd();
   gEPICSCurrent->Draw("alp");
   graph_df::SetLabels(gEPICSCurrent,"","Time [sec]","IBC1H04CRCUR2 [#muA]"); 
   lo[7]->Draw("same"); 
   hi[7]->Draw("same");
   c3->Update();  

   return 0;
}
