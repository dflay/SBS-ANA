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
#include "./src/BeamManager.cxx"

int beamPlot(const char *runPath){

   // settings 
   bool logScale   = false;

   gStyle->SetOptStat(0);

   int rc=0;

   TString prefix;
   std::vector<codaRun_t> runList;  
   rc = bcm_util::LoadRuns(runPath,prefix,runList);
   if(rc!=0) return 1; 

   BeamManager *mgr = new BeamManager("NONE",false,"NONE");

   TString filePath;  
   const int NR = runList.size();  
   for(int i=0;i<NR;i++){ 
      std::cout << "Loading run " << runList[i].runNumber << std::endl;
      filePath = Form("%s/gmn_replayed-beam_%d_stream%d_seg%d_%d.root",
                      prefix.Data(),runList[i].runNumber,runList[i].stream,runList[i].segmentBegin,runList[i].segmentEnd);
      mgr->LoadFile(filePath,runList[i].runNumber);
   }

   const int N = 8; 
   TString varName[N] = {"Raster.rawcur.x","Raster.rawcur.y","Raster2.rawcur.x","Raster2.rawcur.y",
                         "BPMA.rawcur.1"  ,"BPMA.rawcur.2"  ,"BPMB.rawcur.1"   ,"BPMB.rawcur.2"};
   TString xAxis      = Form("event"); 

   // create graphs and histograms 

   TGraph **g = new TGraph*[N];
   TH1F **h   = new TH1F*[N]; 

   int NBin = 100; 
   // histo bounds  u1  unew    unser  dnew d1     d3    d10  
   double min[N] = {0     ,0     ,0     ,0     ,0     ,0     ,0     };
   double max[N] = {100E+3,100E+3,100E+3,100E+3,100E+3,100E+3,100E+3};

   for(int i=0;i<N;i++){
      g[i] = mgr->GetTGraph("SBSrb",xAxis.Data(),varName[i]);
      graph_df::SetParameters(g[i],20,kBlack);
      h[i] = mgr->GetTH1F("SBSrb",varName[i].Data(),NBin,min[i],max[i]); 
   }

   // 2D raster plot, BPM plot
   int NBin2D = 4000;  
   TH2F *r2D_1 = mgr->GetTH2F("SBSrb","Raster.rawcur.x" ,"Raster.rawcur.y" ,NBin2D,0,95E+3,NBin2D,0,95E+3);
   TH2F *r2D_2 = mgr->GetTH2F("SBSrb","Raster2.rawcur.x","Raster2.rawcur.y",NBin2D,0,95E+3,NBin2D,0,95E+3);
   TH2F *p2D_A = mgr->GetTH2F("SBSrb","BPMA.x"  ,"BPMA.y"  ,NBin2D,-10E-3,10E-3,NBin2D,-10E-3,10E-3);
   TH2F *p2D_B = mgr->GetTH2F("SBSrb","BPMB.x"  ,"BPMB.y"  ,NBin2D,-10E-3,10E-3,NBin2D,-10E-3,10E-3);
   TH2F *tgt   = mgr->GetTH2F("SBSrb","target.x","target.y",NBin2D,-4,4,NBin2D,-4,4);

   TCanvas *c1a = new TCanvas("c1a","Beam Check",1200,800);
   c1a->Divide(2,2);

   TCanvas *c1b = new TCanvas("c1b","Beam Check",1200,800);
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
   TCanvas *c2a = new TCanvas("c2a","Beam Check [Histograms]",1200,800);
   c2a->Divide(2,2);

   TCanvas *c2b = new TCanvas("c2b","Beam Check [Histograms]",1200,800);
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

   TCanvas *c3 = new TCanvas("c3","Raster and BPM Plots",1200,800);
   c3->Divide(2,2);

   c3->cd(1); 
   r2D_1->Draw("colz");
   c3->Update();

   c3->cd(2); 
   r2D_2->Draw("colz");
   c3->Update();

   c3->cd(3); 
   p2D_A->Draw("colz");
   c3->Update();

   c3->cd(4); 
   p2D_B->Draw("colz");
   c3->Update();

   TCanvas *c4 = new TCanvas("c4","Beam Position at Target",1200,800);

   c4->cd(); 
   tgt->Draw("colz");
   c4->Update();

   return 0;
}
