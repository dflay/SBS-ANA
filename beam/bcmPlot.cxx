// testing the BCM data 

#include <cstdlib>
#include <iostream>

#include "TTree.h"
#include "TFile.h"
#include "TH1F.h"
#include "TStyle.h"
#include "TPad.h"

#include "./src/BCMPlotter.cxx"

int bcmPlot(){

   // settings 
   bool logScale   = false;
   bool sameCanvas = true;
   
   gStyle->SetOptStat(0);
   
   int run;
   std::cout << "Enter run number: ";
   std::cin  >> run;

   int startSegment = 0; 
   int endSegment   = 0; 

   TString prefix   = Form("/lustre19/expphy/volatile/halla/sbs/flay/GMnAnalysis/rootfiles");
   TString filePath = Form("%s/gmn_replayed-beam_%d_stream0_seg%d_%d.root",prefix.Data(),run,startSegment,endSegment);

   BCMPlotter *myPlotter = new BCMPlotter();
   myPlotter->LoadFile(filePath); 

   const int N = 7; 
   TString varName[N] = {"u1","unew","unser","dnew","d1","d3","d10"};

   int NBin = 100;
   // histo bounds        u1  unew unser  dnew  d1     d3  d10
   double loBeamOff[N] = {50  ,0  ,700E+3,0    ,1E+3  ,0  ,0  };
   double hiBeamOff[N] = {150,100 ,850E+3,20E+3,1.5E+3,100,100};

   double timeMin = 0; 
   double timeMax = 150; 

   TString hName,hRange;

   TH2F **h = new TH2F*[N]; 
   for(int i=0;i<N;i++){
      hName  = Form("h%d",i+1);
      hRange = Form("(%d,%.0lf,%.0lf,%d,%.0lf,%.0lf)",NBin,timeMin,timeMax,NBin,loBeamOff[i],hiBeamOff[i]); 
      h[i]   = myPlotter->GetTH2F(hName,hRange,"sbs","BBCalHi.RF",varName[i],"rate");;
      h[i]->SetMarkerStyle(20); 
      h[i]->SetTitle(Form("%s",varName[i].Data())); 
      h[i]->GetXaxis()->SetTitle("Time [sec]"); 
      h[i]->GetXaxis()->CenterTitle(); 
      h[i]->GetYaxis()->SetTitle(Form("%s [Hz]",varName[i].Data())); 
      h[i]->GetYaxis()->CenterTitle(); 
   }

   TCanvas *c1a = new TCanvas("c1a","BCM Check: LHRS and SBS",1200,800);
   c1a->Divide(2,2);

   TCanvas *c1b = new TCanvas("c1b","BCM Check: LHRS and SBS",1200,800);
   c1b->Divide(2,2);

   for(int i=0;i<N/2;i++){
      c1a->cd(i+1);
      h[i]->Draw();
      c1a->Update();
      c1b->cd(i+1);
      h[i+3]->Draw();
      c1b->Update();
   }

   // last one 
   c1b->cd(4); 
   h[6]->Draw();
   c1b->Update();

   return 0;
}

