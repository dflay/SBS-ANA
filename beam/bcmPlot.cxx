// testing the BCM data 

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
   bool sameCanvas = true;
   
   gStyle->SetOptStat(0);
 
   int rc=0;

   TString prefix; 
   std::vector<int> runList;
   rc = bcm_util::LoadRuns("./input/run-list.csv",prefix,runList);
   if(rc!=0) return 1; 
   
   BCMPlotter *myPlotter = new BCMPlotter();
   myPlotter->SetDebug();
   // myPlotter->EnableEPICS(); 

   TString filePath;  
   int startSegment = 0; 
   int endSegment   = 0; 
   const int NR = runList.size();  
   for(int i=0;i<NR;i++){ 
      filePath = Form("%s/gmn_replayed-beam_%d_stream0_seg%d_%d.root",prefix.Data(),runList[i],startSegment,endSegment);
      myPlotter->LoadFile(filePath);
   } 

   std::vector<cut_t> cutList; 
   rc = bcm_util::LoadCuts("./input/cut-list.csv",cutList); 
   if(rc!=0) return 1; 

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
   
   TString theVar; 
   double mean=0,stdev=0;
   double CUT_LO=0,CUT_HI=0;
   std::vector<double> v; 

   const int NC = cutList.size();
   TLine **lo = new TLine*[NC]; 
   TLine **hi = new TLine*[NC]; 

   // let's do cuts on each variable defined 
   for(int i=0;i<NC;i++){
      // define variable and get a vector of all data  
      theVar = Form("sbs.bcm.%s.rate",cutList[i].dev.c_str());
      myPlotter->GetVector("sbs",theVar.Data(),v); 
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
   }
 
   // create histos and TGraphs 
 
   TString hName,hRange;

   TGraph **g = new TGraph*[N]; 
   TH2F **h   = new TH2F*[N]; 

   for(int i=0;i<N;i++){
      hName  = Form("h%d",i+1);
      hRange = Form("(%d,%.0lf,%.0lf,%d,%.0lf,%.0lf)",NBin,timeMin,timeMax,NBin,loBeamOff[i],hiBeamOff[i]); 
      h[i]   = myPlotter->GetTH2F(hName,hRange,"sbs","BBCalHi.RF",varName[i],"rate");
      h[i]->SetMarkerStyle(20); 
      h[i]->SetTitle(Form("%s",varName[i].Data())); 
      h[i]->GetXaxis()->SetTitle("Time [sec]"); 
      h[i]->GetXaxis()->CenterTitle(); 
      h[i]->GetYaxis()->SetTitle(Form("%s [Hz]",varName[i].Data())); 
      h[i]->GetYaxis()->CenterTitle(); 
      g[i] = myPlotter->GetTGraph("sbs","time",varName[i],"rate");
   }

   // TGraph *gEPICSCurrent = myPlotter->GetTGraph("E","time","IBC1H04CRCUR2",""); 
   // gEPICSCurrent->SetMarkerStyle(20);

   // TCanvas *c3 = new TCanvas("c3","EPICS Beam Current",1200,800); 

   // c3->cd();
   // gEPICSCurrent->Draw("alp");
   // gEPICSCurrent->GetXaxis()->SetTitle("Time");  
   // gEPICSCurrent->GetXaxis()->CenterTitle();  
   // gEPICSCurrent->GetYaxis()->SetTitle("IBC1H04CRCUR2");  
   // gEPICSCurrent->GetYaxis()->CenterTitle();  
   // c3->Update();  

   // TCanvas *c1a = new TCanvas("c1a","BCM Check: LHRS and SBS",1200,800);
   // c1a->Divide(2,2);

   // TCanvas *c1b = new TCanvas("c1b","BCM Check: LHRS and SBS",1200,800);
   // c1b->Divide(2,2);

   // for(int i=0;i<N/2;i++){
   //    c1a->cd(i+1);
   //    h[i]->Draw();
   //    c1a->Update();
   //    c1b->cd(i+1);
   //    h[i+3]->Draw();
   //    c1b->Update();
   // }

   // // last one 
   // c1b->cd(4); 
   // h[6]->Draw();
   // c1b->Update();

   TCanvas *c2a = new TCanvas("c2a","BCM Check",1200,800);
   c2a->Divide(2,2);

   TCanvas *c2b = new TCanvas("c2b","BCM Check",1200,800);
   c2b->Divide(2,2);

   for(int i=0;i<N/2;i++){
      c2a->cd(i+1);
      g[i]->Draw();
      g[i]->SetMarkerStyle(20); 
      g[i]->SetTitle(Form("%s",varName[i].Data())); 
      g[i]->GetXaxis()->SetTitle("Time [sec]"); 
      g[i]->GetXaxis()->CenterTitle(); 
      g[i]->GetYaxis()->SetTitle(Form("%s [Hz]",varName[i].Data())); 
      g[i]->GetYaxis()->CenterTitle(); 
      g[i]->Draw();
      lo[i]->Draw("same"); 
      hi[i]->Draw("same");
      c2a->Update();
      c2b->cd(i+1);
      g[i+3]->Draw();
      g[i+3]->SetMarkerStyle(20); 
      g[i+3]->SetTitle(Form("%s",varName[i+3].Data())); 
      g[i+3]->GetXaxis()->SetTitle("Time [sec]"); 
      g[i+3]->GetXaxis()->CenterTitle(); 
      g[i+3]->GetYaxis()->SetTitle(Form("%s [Hz]",varName[i+3].Data())); 
      g[i+3]->GetYaxis()->CenterTitle(); 
      g[i+3]->Draw();
      lo[i+3]->Draw("same"); 
      hi[i+3]->Draw("same");
      c2b->Update();
   }

   // last one 
   c2b->cd(4); 
   g[6]->Draw();
   g[6]->SetMarkerStyle(20); 
   g[6]->SetTitle(Form("%s",varName[6].Data())); 
   g[6]->GetXaxis()->SetTitle("Time [sec]"); 
   g[6]->GetXaxis()->CenterTitle(); 
   g[6]->GetYaxis()->SetTitle(Form("%s [Hz]",varName[6].Data())); 
   g[6]->GetYaxis()->CenterTitle(); 
   g[6]->Draw();
   lo[6]->Draw("same"); 
   hi[6]->Draw("same");
   c2b->Update();

   return 0;
}
