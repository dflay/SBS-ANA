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

   // get run stats for all variables
   int M=0;
   for(int i=0;i<N;i++){
      bcm_util::GetStats_byRun(var[i].Data(),data,run,mean,stdev);
      std::cout << Form("%s:",var[i].Data()) << std::endl; 
      M = mean.size();
      std::cout << "-- without cuts -- " << std::endl;
      for(int j=0;j<M;j++){
	 std::cout << Form("   run = %d, mean = %.3lf, stdev = %.3lf",(int)run[j],mean[j],stdev[j]) << std::endl;
      }
      // now with cuts 
      run.clear();
      mean.clear();
      stdev.clear();
      bcm_util::GetStats_byRun(var[i].Data(),DATA,run,mean,stdev);
      std::cout << "-- with cuts -- " << std::endl;
      for(int j=0;j<M;j++){
	 std::cout << Form("   run = %d, mean = %.3lf, stdev = %.3lf",(int)run[j],mean[j],stdev[j]) << std::endl;
      }
      std::cout << Form("====================================================") << std::endl;
      // set up for next run 
      run.clear();
      mean.clear();
      stdev.clear();
   } 

   // std::vector<double> run;
   // std::vector<double> mean_uns,stdev_uns;
   // std::vector<double> mean_unsi,stdev_unsi;
   // std::vector<double> mean_unew,stdev_unew;
   // std::vector<double> mean_u1,stdev_u1;
   // std::vector<double> mean_d1,stdev_d1;
   // std::vector<double> mean_d3,stdev_d3;
   // std::vector<double> mean_d10,stdev_d10;
   // std::vector<double> mean_dnew,stdev_dnew;
   
   // bcm_util::GetStats_byRun(var[0].Data(),data,run,mean_uns ,stdev_uns );
   // run.clear();
   // bcm_util::GetStats_byRun(var[1].Data(),data,run,mean_u1  ,stdev_u1  );
   // run.clear();
   // bcm_util::GetStats_byRun(var[2].Data(),data,run,mean_unew,stdev_unew);
   // run.clear();
   // bcm_util::GetStats_byRun(var[3].Data(),data,run,mean_d1  ,stdev_d1  );
   // run.clear();
   // bcm_util::GetStats_byRun(var[4].Data(),data,run,mean_d3  ,stdev_d3  );
   // run.clear();
   // bcm_util::GetStats_byRun(var[5].Data(),data,run,mean_d10 ,stdev_d10 );
   // run.clear();
   // bcm_util::GetStats_byRun(var[6].Data(),data,run,mean_dnew,stdev_dnew);
   // run.clear();
   // bcm_util::GetStats_byRun("unser.current",data,run,mean_unsi,stdev_unsi);

   // if we're missing runs in the initial run list, 
   // the analyzed run list is different (smaller) 
   // int NNR = run.size();

   // print to screen
   // for(int i=0;i<NNR;i++){
// //       std::cout << Form("run %05d: unser = %.3lf ± %.3lf, u1 = %.3lf ± %.3lf, unew = %.3lf ± %.3lf, d1 = %.3lf ± %.3lf, d3 = %.3lf ± %.3lf, d10 = %.3lf ± %.3lf, dnew = %.3lf ± %.3lf",
// // 	    (int)run[i],mean_uns[i],stdev_uns[i],mean_u1[i],stdev_u1[i],mean_unew[i],stdev_unew[i],mean_d1[i],stdev_d1[i],mean_d3[i],stdev_d3[i],mean_d10[i],stdev_d10[i],mean_dnew[i],stdev_dnew[i]) << std::endl;
   //    std::cout << Form("%05d,%.3lf,%.3lf,%.3lf,%.3lf,%.3lf,%.3lf,%.3lf,%.3lf,%.3lf,%.3lf,%.3lf,%.3lf,%.3lf,%.3lf,%.3lf,%.3lf",
   //                      (int)run[i],mean_uns[i],stdev_uns[i],mean_unsi[i],stdev_unsi[i],mean_u1[i],stdev_u1[i],
   //                      mean_unew[i],stdev_unew[i],mean_d1[i],stdev_d1[i],mean_d3[i],stdev_d3[i],
   //                      mean_d10[i],stdev_d10[i],mean_dnew[i],stdev_dnew[i]) << std::endl;
   // }

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
      gb[i]  = bcm_util::GetTGraph("runEvent",var[i].Data(),data); 
      ga[i]  = bcm_util::GetTGraph("runEvent",var[i].Data(),DATA); 
      gac[i] = bcm_util::GetTGraph("runEvent",varC[i].Data(),DATA); 
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

   TString canvasTitle = Form("Run %d: BCM Check",(int)rr[0]);
  
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

   TCanvas *c2 = new TCanvas("c2","EPICS Current",1200,800);
   c2->cd();

   gEPICSCurrent->Draw("ap");
   graph_df::SetLabels(gEPICSCurrent,"EPICS Current","Event Number","EPICS Current [#muA]"); 
   gEPICSCurrent->Draw("ap");
   c2->Update();

   TCanvas *c3 = new TCanvas("c3","Unser and BCM Current",1200,800);
   c3->cd();

   mgc->Draw("a"); 
   graph_df::SetLabels(mgc,"Beam Currents","Run Event Number","Beam Current [#muA]"); 
   mgc->Draw("a");
   L->Draw("same"); 
   c3->Update(); 

   delete mgr;
   delete jmgr; 

   return 0;
}

