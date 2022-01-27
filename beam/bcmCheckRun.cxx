// Plot all BCM data vs run number  

#include <cstdlib>
#include <iostream>

#include "TTree.h"
#include "TFile.h"
#include "TH1F.h"
#include "TStyle.h"
#include "TLine.h"

#include "./include/codaRun.h"
#include "./src/Utilities.cxx"
#include "./src/JSONManager.cxx"
#include "./src/Graph.cxx"
#include "./src/bcmUtilities.cxx"
#include "./src/cutUtilities.cxx"

TString GetTitle(std::vector<int> run); 

int GetStats(std::vector<std::string> var,std::vector<scalerData_t> data,
             std::vector<double> &run,std::vector<std::vector<double>> &mean,std::vector<std::vector<double>> &stdev); 

int bcmCheckRun(const char *confPath){

   // settings 
   bool logScale = false;

   gStyle->SetOptStat(0);

   int rc=0;

   // read input configuration file 
   JSONManager *jmgr = new JSONManager(confPath);
   std::string prefix       = jmgr->GetValueFromKey_str("ROOTfile-path"); 
   std::string runPath      = jmgr->GetValueFromKey_str("run-path");  
   std::string cutPath      = jmgr->GetValueFromKey_str("cut-path"); 
   std::string bcmCalibPath = jmgr->GetValueFromKey_str("bcm-cc-path");   

   std::vector<codaRun_t> runList;  
   rc = bcm_util::LoadRuns(runPath.c_str(),runList);
   if(rc!=0) return 1; 

   BCMManager *mgr = new BCMManager("NONE",bcmCalibPath.c_str(),false);

   std::vector<int> md;
   TString filePath;  
   const int NR = runList.size();  
   for(int i=0;i<NR;i++){ 
      std::cout << Form("Loading run %d",runList[i].runNumber) << std::endl;
      util_df::GetROOTFileMetaData(prefix.c_str(),runList[i].runNumber,md);
      runList[i].stream       = md[0];  
      runList[i].segmentBegin = md[1];  
      runList[i].segmentEnd   = md[2];  
      filePath = Form("%s/gmn_replayed-beam_%d_stream%d_seg%d_%d.root",
                      prefix.c_str(),runList[i].runNumber,runList[i].stream,runList[i].segmentBegin,runList[i].segmentEnd);
      mgr->LoadFile(filePath,runList[i].runNumber);
      md.clear();
   }

   // do stats by run number 
   std::vector<scalerData_t> rawData,data,RAW_DATA,DATA;
   mgr->GetVector_scaler("sbs",rawData);  

   std::vector<epicsData_t> erawData,edata,ERAW_DATA,EDATA; 
   mgr->GetVector_epics(erawData);  

   // load cuts 
   std::vector<cut_t> cutList; 
   cut_util::LoadCuts(cutPath.c_str(),cutList);
   // apply cuts  
   cut_util::ApplyCuts(cutList,rawData,data);

   // sort the data by run number 
   std::sort(data.begin(),data.end(),compareScalerData_byRun); 

   std::vector<int> rr; 
   mgr->GetRunList(rr); 
   const int NNR = rr.size();

   char inpath_cuts[200]; 
   std::vector<cut_t> runCuts;

   // load and apply cuts
   int NEV=0,N_CUTS=0; 
   for(int i=0;i<NNR;i++){
      // grab a run and apply a list of cuts 
      mgr->GetVector_scaler("sbs",rr[i],RAW_DATA);
      // load and apply cuts 
      sprintf(inpath_cuts,"./input/cut-list/bcm-calib/run-%d.json",rr[i]);
      cut_util::LoadCuts_json(inpath_cuts,runCuts);
      N_CUTS = runCuts.size();
      if(N_CUTS>0){
	 cut_util::ApplyCuts_alt(runCuts,RAW_DATA,DATA);
      }else{
	 // copy all data, no cuts
	 NEV = RAW_DATA.size();
	 for(int j=0;j<NEV;j++) DATA.push_back(RAW_DATA[j]);
      }
      // reset 
      runCuts.clear();
      RAW_DATA.clear();  
   }

   const int N = 7; 
   TString var[N]  = {"unser.rate"   ,"u1.rate"   ,"unew.rate"   ,"d1.rate"   ,"d3.rate"   ,"d10.rate"   ,"dnew.rate"};
   TString varC[N] = {"unser.current","u1.current","unew.current","d1.current","d3.current","d10.current","dnew.current"};

   std::vector<double> run,mean,stdev;
   std::vector<std::vector<double>> mm,ss;   // no cuts
   std::vector<std::vector<double>> mmc,ssc; // with cuts 

   // get run stats for all variables
   int M=0;
   for(int i=0;i<N;i++){
      bcm_util::GetStats_byRun(varC[i].Data(),data,run,mean,stdev);
      // store results
      mm.push_back(mean); 
      ss.push_back(stdev); 
      // now with cuts 
      run.clear();
      mean.clear();
      stdev.clear();
      bcm_util::GetStats_byRun(varC[i].Data(),DATA,run,mean,stdev);
      // store results
      mmc.push_back(mean); 
      ssc.push_back(stdev); 
      // set up for next variable 
      run.clear();
      mean.clear();
      stdev.clear();
   } 

   // print results to screen
   for(int i=0;i<N;i++){
      std::cout << Form("%s: ",varC[i].Data()) << std::endl;
      M = mm[i].size(); // run dimension 
      for(int j=0;j<M;j++){
	 std::cout << Form("   run %d: no cuts = %.3lf ± %.3lf, with cuts = %.3lf ± %.3lf",
                           rr[j],mm[i][j],ss[i][j],mmc[i][j],ssc[i][j]) << std::endl;
      }
   } 

   // get BCM plots vs event number
   TLegend *L = new TLegend(0.6,0.6,0.8,0.8); 
   int color[N] = {kBlack,kRed,kBlue,kGreen+2,kMagenta,kCyan+2,kOrange}; 
   TMultiGraph **mg  = new TMultiGraph*[N]; 
   TMultiGraph *mgc = new TMultiGraph(); 
   TGraph **gb  = new TGraph*[N]; 
   TGraph **ga  = new TGraph*[N]; 
   TGraph **gac = new TGraph*[N]; 
   for(int i=0;i<N;i++){
      mg[i]  = new TMultiGraph();
      gb[i]  = bcm_util::GetTGraph("event",var[i].Data(),data); 
      ga[i]  = bcm_util::GetTGraph("event",var[i].Data(),DATA); 
      gac[i] = bcm_util::GetTGraph("event",varC[i].Data(),DATA); 
      graph_df::SetParameters(gb[i],21,kBlack);
      graph_df::SetParameters(ga[i],20,kRed);
      graph_df::SetParameters(gac[i],20,color[i]);
      mg[i]->Add(gb[i],"p"); 
      mg[i]->Add(ga[i],"p");
      mgc->Add(gac[i],"p"); 
      L->AddEntry(gac[i],varC[i].Data(),"p"); 
   }

   TString Title,yAxisTitle;
   TString xAxisTitle = Form("Run Event Number");

   TString canvasTitle = GetTitle(rr); 
  
   TCanvas *c1a = new TCanvas("c1a",canvasTitle,1200,800);
   c1a->Divide(2,2);

   TCanvas *c1b = new TCanvas("c1b",canvasTitle,1200,800);
   c1b->Divide(2,2); 

   for(int i=0;i<N/2;i++){
      c1a->cd(i+1);
      Title      = Form("%s"     ,var[i].Data());
      yAxisTitle = Form("%s [Hz]",var[i].Data());
      mg[i]->Draw("alp");
      graph_df::SetLabels(mg[i],Title,xAxisTitle,yAxisTitle);
      mg[i]->Draw("alp");
      c1a->Update();
      // next canvas 
      c1b->cd(i+1);
      Title      = Form("%s"     ,var[i+3].Data());
      yAxisTitle = Form("%s [Hz]",var[i+3].Data());
      mg[i+3]->Draw("alp");
      graph_df::SetLabels(mg[i+3],Title,xAxisTitle,yAxisTitle);
      mg[i+3]->Draw("alp");
      c1b->Update();
   }

   // last one 
   Title      = Form("%s"     ,var[6].Data());
   yAxisTitle = Form("%s [Hz]",var[6].Data());
   c1b->cd(4);
   mg[6]->Draw("alp");
   graph_df::SetLabels(mg[6],Title,xAxisTitle,yAxisTitle);
   mg[6]->Draw("alp");
   c1b->Update();

   // make a plot of the beam current from EPICS 
   TGraph *gEPICSCurrent = mgr->GetTGraph("E","event","IBC1H04CRCUR2");
   graph_df::SetParameters(gEPICSCurrent,20,kBlack);   

   Title = GetTitle(rr); 

   TCanvas *c3 = new TCanvas("c3","Unser and BCM Current",1200,800);
   c3->Divide(1,2);

   c3->cd(1);
   mgc->Draw("a"); 
   graph_df::SetLabels(mgc,Title,"Scaler Event Number","Beam Current [#muA]");
   graph_df::SetLabelSizes(mgc,0.05,0.06);  
   mgc->Draw("a");
   L->Draw("same"); 
   c3->Update(); 

   c3->cd(2);
   gEPICSCurrent->Draw("ap");
   graph_df::SetLabels(gEPICSCurrent,"EPICS Current","EPICS Event Number","Beam Current [#muA]"); 
   graph_df::SetLabelSizes(gEPICSCurrent,0.05,0.06);  
   gEPICSCurrent->Draw("ap");
   c3->Update();

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
//______________________________________________________________________________
int GetStats(std::vector<std::string> var,std::vector<scalerData_t> data,
             std::vector<double> &run,std::vector<std::vector<double>> &mean,std::vector<std::vector<double>> &stdev){
   // get mean and stdev for all BCMs for all runs  
   std::vector<double> mm,ss; 
   const int NV = var.size();
   for(int i=0;i<NV;i++){
      bcm_util::GetStats_byRun(var[i].c_str(),data,run,mm,ss);
      mean.push_back(mm);
      stdev.push_back(ss);
      // set up for next BCM variable
      mm.clear();
      ss.clear(); 
      if(i!=NV-1) run.clear(); // delete until last value  
   }
   return 0;
}
