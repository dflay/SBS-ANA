// Plot all BCM data  

#include <cstdlib>
#include <iostream>

#include "TTree.h"
#include "TFile.h"
#include "TH1F.h"
#include "TStyle.h"
#include "TPad.h"
#include "TLine.h"

#include "./include/cut.h"
#include "./src/BCMPlotter.cxx"
#include "./src/bcmUtilities.cxx"

int bcmPlot(){

   // settings 
   bool logScale   = false;
   
   gStyle->SetOptStat(0);
 
   int rc=0;

   TString prefix; 
   std::vector<int> runList;
   rc = bcm_util::LoadRuns("./input/run-list.csv",prefix,runList);
   if(rc!=0) return 1; 
   
   BCMPlotter *myPlotter = new BCMPlotter();
   // myPlotter->SetDebug();
   myPlotter->EnableEPICS(); 

   TString filePath;  
   int startSegment = 0; 
   int endSegment   = 0; 
   const int NR = runList.size();  
   for(int i=0;i<NR;i++){ 
      filePath = Form("%s/gmn_replayed-beam_%d_stream0_seg%d_%d.root",prefix.Data(),runList[i],startSegment,endSegment);
      myPlotter->LoadFile(filePath);
   } 

   const int N = 7; 
   TString varName[N] = {"u1","unew","unser","dnew","d1","d3","d10"};

   int NBin = 100;
   // histo bounds        u1  unew unser  dnew  d1     d3  d10
   double loBeamOff[N] = {50  ,0  ,700E+3,0    ,1E+3  ,0  ,0  };
   double hiBeamOff[N] = {150,100 ,850E+3,20E+3,1.5E+3,100,100};

   // test getting a vector of data 
   std::vector<double> time; 
   myPlotter->GetVector("sbs","time",time); 

   const int NT = time.size();
   double timeMin = time[0];
   double timeMax = time[NT-1]; 
 
   // create histos and TGraphs 

   TGraph **g = new TGraph*[N]; 

   for(int i=0;i<N;i++){
      g[i] = myPlotter->GetTGraph("sbs","time",varName[i],"rate");
   }

   TGraph *gEPICSCurrent = myPlotter->GetTGraph("E","time","IBC1H04CRCUR2",""); 
   gEPICSCurrent->SetMarkerStyle(20);

   TCanvas *c1a = new TCanvas("c1a","BCM Check",1200,800);
   c1a->Divide(2,2);

   TCanvas *c1b = new TCanvas("c1b","BCM Check",1200,800);
   c1b->Divide(2,2);

   for(int i=0;i<N/2;i++){
      c1a->cd(i+1);
      g[i]->Draw("alp");
      g[i]->SetMarkerStyle(20); 
      g[i]->SetTitle(Form("%s",varName[i].Data())); 
      g[i]->GetXaxis()->SetTitle("Time [sec]"); 
      g[i]->GetXaxis()->CenterTitle(); 
      g[i]->GetYaxis()->SetTitle(Form("%s [Hz]",varName[i].Data())); 
      g[i]->GetYaxis()->CenterTitle(); 
      g[i]->Draw("alp");
      c1a->Update();
      c1b->cd(i+1);
      g[i+3]->Draw("alp");
      g[i+3]->SetMarkerStyle(20); 
      g[i+3]->SetTitle(Form("%s",varName[i+3].Data())); 
      g[i+3]->GetXaxis()->SetTitle("Time [sec]"); 
      g[i+3]->GetXaxis()->CenterTitle(); 
      g[i+3]->GetYaxis()->SetTitle(Form("%s [Hz]",varName[i+3].Data())); 
      g[i+3]->GetYaxis()->CenterTitle(); 
      g[i+3]->Draw("alp");
      c1b->Update();
   }

   // last one 
   c1b->cd(4); 
   g[6]->Draw("alp");
   g[6]->SetMarkerStyle(20); 
   g[6]->SetTitle(Form("%s",varName[6].Data())); 
   g[6]->GetXaxis()->SetTitle("Time [sec]"); 
   g[6]->GetXaxis()->CenterTitle(); 
   g[6]->GetYaxis()->SetTitle(Form("%s [Hz]",varName[6].Data())); 
   g[6]->GetYaxis()->CenterTitle(); 
   g[6]->Draw("alp");
   c1b->Update();

   // EPICS plots

   TCanvas *c3 = new TCanvas("c3","EPICS Beam Current",1200,800); 

   c3->cd();
   gEPICSCurrent->Draw("alp");
   gEPICSCurrent->GetXaxis()->SetTitle("Time");  
   gEPICSCurrent->GetXaxis()->CenterTitle();  
   gEPICSCurrent->GetYaxis()->SetTitle("IBC1H04CRCUR2");  
   gEPICSCurrent->GetYaxis()->CenterTitle();  
   gEPICSCurrent->Draw("alp");
   c3->Update();  

   return 0;
}
