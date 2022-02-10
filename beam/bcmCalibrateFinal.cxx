// Compute BCM calibration coefficients
// - Uses the output of bcmCalibProcessCuts.cxx
// - Computes (beam-on) - (beam-off) BCM rates (all variables) 
//   and plots as a function of the Unser current.
//   Linear fit produces the calibration coefficients  
//
//   TODO
//   1. Print fit results to screen  

#include <cstdlib>
#include <iostream> 

#include "TTree.h"
#include "TFile.h"
#include "TH1F.h"
#include "TStyle.h"
#include "TPad.h"
#include "TLine.h"

#include "./include/producedVariable.h"
#include "./include/calibCoeff.h"
#include "./include/codaRun.h"
#include "./src/ABA.cxx"
#include "./src/Graph.cxx"
#include "./src/CSVManager.cxx"
#include "./src/JSONManager.cxx"
#include "./src/bcmUtilities.cxx"

double myFitFunc(double *x,double *p); 
int CalculatePedestalSubtraction(std::vector<producedVariable_t> data,std::vector<producedVariable_t> &out); 
int ConvertToCurrent(calibCoeff_t cc,std::vector<producedVariable_t> unser_ps,
                     std::vector<producedVariable_t> &unser_cur); 

TGraphErrors *GetTGraphErrors(std::vector<producedVariable_t> unser,std::vector<producedVariable_t> bcm); 

int bcmCalibrateFinal(const char *confPath){

   std::cout << "========== COMPUTING BCM CALIBRATION COEFFICIENTS ==========" << std::endl;

   // load configuration file 
   JSONManager *jmgr = new JSONManager(confPath);
   std::string unsccPath = jmgr->GetValueFromKey_str("unser-cc-path"); 
   std::string tag       = jmgr->GetValueFromKey_str("tag"); 

   char data_dir[200],plot_dir[200];
   sprintf(data_dir,"./output/%s",tag.c_str());
   sprintf(plot_dir,"./plots/%s" ,tag.c_str());

   std::vector<double> fitMin,fitMax; 
   jmgr->GetVectorFromKey<double>("fit-min",fitMin); 
   jmgr->GetVectorFromKey<double>("fit-max",fitMax); 
   delete jmgr;

   // load Unser data 
   char unserPath[200]; 
   sprintf(unserPath,"%s/unser.csv",data_dir); 
   std::vector<producedVariable_t> unser;
   rc = bcm_util::LoadProducedVariables(unserPath,unser);
   if(rc!=0) return 1;

   // compute pedestal subtracted Unser rates, convert to current
   std::vector<producedVariable_t> unser_ps; 
   CalculatePedestalSubtraction(unser,unser_ps); 
   // load Unser calibration coefficients 
   std::vector<calibCoeff_t> unsCC;
   LoadFittedOffsetGainData(unsccPath.c_str(),unsCC); 
   // convert to current 
   std::vector<producedVariable_t> unser_cur;
   ConvertToCurrent(unsCC[0],unser_ps,unser_cur); // only ONE set of unser calibration coefficients!  

   // load each BCM variable; compute pedestal-subtracted 
   // values. create plots of ped-subtracted rates vs Unser current 
   const int N = 6; 
   std::string bcmVar[N] = {"u1","unew","d1","d3","d10","dnew"};

   TGraphErrors **g = new TGraphErrors*[N]; 
   TF1 **myFit      = new TF1*[N]; 
 
   // set up fit parameters
   const int npar=2;
   double offset=0,offsetErr=0,slope=0,slopeErr=0; 

   // calibration coefficient output 
   calibCoeff_t ccPt; 
   std::vector<calibCoeff_t> cc; 

   int NCOL=3,NROW=2;
   TCanvas *c1 = new TCanvas("c1","BCM Calibration",1200,800);
   c1->Divide(NCOL,NROW);  

   TString Title,xAxisTitle,yAxisTitle,fitName;

   char inpath[200]; 
   
   std::vector<producedVariable_t> bcm,bcm_ps; 
   for(int i=0;i<N;i++){
      // load data
      sprintf(inpath,"%s/%s.csv",data_dir,bcmVar[i].c_str()); 
      rc = bcm_util::LoadProducedVariables(inpath,bcm);
      // subtract pedestal
      CalculatePedestalSubtraction(bcm,bcm_ps);
      // create TGraphError plot  
      g[i] = GetTGraphErrors(unser_cur,bcm_ps); 
      graph_df::SetParameters(g[i],20,kBlack);
      // set up the fit function 
      fitName = Form("fit_%s",bcmVar[i].c_str());
      myFit[i] = new TF1(fitName,myFitFunc,fitMin[i],fitMax[i],npar);
      for(int j=0;j<npar;j++) myFit->SetParameter(j,0);
      // set titles 
      Title      = Form("%s"                  ,bcmVar[i].c_str());
      xAxisTitle = Form("Unser Current [#muA]",bcmVar[i].c_str());
      yAxisTitle = Form("%s rate [Hz]"        ,bcmVar[i].c_str());
      // plot data 
      c1->cd(i+1);  
      g[i]->Draw("ap"); 
      graph_df::SetLabels(g[i],Title,xAxisTitle,yAxisTitle);
      g[i]->Draw("ap");
      // fit the data
      g[i]->Fit(fitName,"QR");
      c1->Update(); 
      // extract fit results
      offset    = myFit[i]->GetParameter(0); 
      offsetErr = myFit[i]->GetParError(0); 
      slope     = myFit[i]->GetParameter(1); 
      slopeErr  = myFit[i]->GetParError(1); 
      // store results 
      ccPt.dev       = bcmVar[i];
      ccPt.offset    = offset; 
      ccPt.offsetErr = offsetErr;
      ccPt.slope     = slope;
      ccPt.slopeErr  = slopeErr;
      cc.push_back(ccPt);
      // print to screen 
      std::cout << Form("bcm = %s, offset = %.3lf ± %.3lf, slope = %.3lf ± %.3lf",
                        ccPt.dev.c_str(),ccPt.offset,ccPt.offsetErr,ccPt.slope,ccPt.slopeErr) << std::endl;
      // set up for next BCM
      bcm.clear();
      bcm_ps.clear(); 
   }

   std::cout << "----------------" << std::endl;

   // create output directory for plot 
   util_df::MakeDirectory(plot_dir);

   // save the canvas
   TString plotPath = Form("%s/%s.pdf",plot_dir,tag.c_str()); 
   c1->cd();
   c1->Print(plotPath);  

   // print results to file
   char outpath[200];
   sprintf(outpath,"%s/result.csv",data_dir); 
   bcm_util::WriteToFile_cc(outpath,cc);  

   return 0;
}
//______________________________________________________________________________
int ConvertToCurrent(calibCoeff_t cc,std::vector<producedVariable_t> unser_ps,
                     std::vector<producedVariable_t> &unser_cur){
   double T1=0,T2=0
   double current=0,currentErr=0;
   producedVariable_t data;
   const int N = unser_ps.size();
   for(int i=0;i<N;i++){
      // compute current 
      current    = (unser_ps[i].mean - cc.offset)/cc.slope;
      // compute error
      T1         = cc.slope*cc.slope*(unser_ps[i].stdev*unser_ps[i].stdev + cc.offsetErr*cc.offsetErr); 
      T2         = cc.slopeErr*cc.slopeErr*TMath::Power(unser_ps[i].mean - cc.offset,2.) 
      currentErr = TMath::Power(1./cc.slope,2.)*TMath::Sqrt(T1 + T2); 
      // save result
      data.mean  = current;
      data.stdev = currentErr;
      unser_cur.push_back(data); 
   }
   return 0;
}
//______________________________________________________________________________
int CalculatePedestalSubtraction(std::vector<producedVariable_t> data,std::vector<producedVariable_t> &out){
   // compute pedestal-subtracted rates 
   // utilizes ABA method to account for zero-point drift
   // - input:  vector of beam-on and beam-off data (each entry for beam on and off for a given group/beam current) 
   // - output: vector of (beam-on) - (beam-off) results (entry for each group/beam current)  
   ABA *myABA = new ABA();
   myABA->UseTimeWeight();
   myABA->SetVerbosity(1);   

   double mean=0,err=0,stdev=0,argErr=0;
   std::vector<double> w,aba,abaErr; 
   std::vector<double> timeOn,on,onErr;
   std::vector<double> timeOff,off,offErr;

   producedVariable_t aPt; 

   int M=0;
   int grp_prev = data[0].group; // effective beam current  
   const int N = data.size();
   for(int i=0;i<N;i++){
      std::cout << Form("==== GROUP %d ====",data[i].group) << std::endl;
      // check the group 
      if(data[i].group==grp_prev){
	 // group match! store data based on beam state 
	 if(data[i].beam_state.compare("on")==0){
	    timeOn.push_back( data[i].time );
	    on.push_back( data[i].mean );
	    onErr.push_back( data[i].stdev );
	 }else if(data[i].beam_state.compare("off")==0){
	    timeOff.push_back( data[i].time );
	    off.push_back( data[i].mean );
	    offErr.push_back( data[i].stdev );
	 }
      }else{
         // new group! compute ABA stats
	 // check 
	 M = timeOff.size();
	 for(int j=0;j<M;j++){
	    std::cout << Form("off: %.3lf, %.3lf ± %.3lf; on: %.3lf, %.3lf ± %.3lf",
		  timeOff[j],off[j],offErr[j],timeOn[j],on[j],onErr[j]) << std::endl;
	 }
	 if(M==1){
	    std::cout << "**** ONLY ONE CYCLE! GROUP " << grp_prev << std::endl;
	    // account for one cycle
	    mean = on[0] - off[0];
	    err  = TMath::Sqrt( onErr[0]*onErr[0] + offErr[0]*offErr[0] );
	    stdev = err;
	 }else{
	    // compute ABA stats
	    myABA->GetDifference(timeOff,off,offErr,timeOn,on,onErr,aba,abaErr);
	    M = aba.size();
	    for(int j=0;j<M;j++){
	       argErr = abaErr[j]*abaErr[j];
	       if(abaErr[j]!=0){
		  w.push_back(1./argErr);
	       }else{
		  w.push_back(1);
	       }
	    }
	    // compute the weighted mean
	    math_df::GetWeightedMean<double>(aba,w,mean,err);
	    stdev = math_df::GetStandardDeviation<double>(aba);
	    // we compute (A-B), but we actually want B-A
	    mean *= -1;
	 }
         // store results
         aPt.mean  = mean;
	 aPt.stdev = stdev;
         out.push_back(aPt);
	 // set up for next
	 w.clear();
	 aba.clear();
	 abaErr.clear();
	 timeOn.clear();
	 on.clear(); 
	 onErr.clear(); 
	 timeOff.clear();
	 off.clear(); 
	 offErr.clear(); 
         // store this one since it's needed for the next set!  
	 if(data[i].beam_state.compare("on")==0){
	    timeOn.push_back( data[i].time );
	    on.push_back( data[i].mean );
	    onErr.push_back( data[i].stdev );
	 }else if(data[i].beam_state.compare("off")==0){
	    timeOff.push_back( data[i].time );
	    off.push_back( data[i].mean );
	    offErr.push_back( data[i].stdev );
	 }
      }
      grp_prev = data[i].group;
   }

   // get the last index computed 
   M = timeOff.size();
   for(int j=0;j<M;j++){
      std::cout << Form("off: %.3lf, %.3lf ± %.3lf; on: %.3lf, %.3lf ± %.3lf",
            timeOff[j],off[j],offErr[j],timeOn[j],on[j],onErr[j]) << std::endl;
   }
   // new group; compute ABA stats
   w.clear();
   aba.clear();
   abaErr.clear();
   if(M==1){
      std::cout << "**** ONLY ONE CYCLE! GROUP " << grp_prev << std::endl;
      // account for one cycle
      mean = on[0] - off[0];
      err  = TMath::Sqrt( onErr[0]*onErr[0] + offErr[0]*offErr[0] );
      stdev = err;
   }else{
      // compute ABA stats
      myABA->GetDifference(timeOff,off,offErr,timeOn,on,onErr,aba,abaErr);
      M = aba.size();
      for(int j=0;j<M;j++){
	 argErr = abaErr[j]*abaErr[j];
	 if(abaErr[j]!=0){
	    w.push_back(1./argErr);
	 }else{
	    w.push_back(1);
	 }
      }
      // compute the weighted mean
      math_df::GetWeightedMean<double>(aba,w,mean,err);
      stdev = math_df::GetStandardDeviation<double>(aba);
      // we compute (A-B), but we actually want B-A
      mean *= -1;
   }
   // store results
   aPt.mean  = mean;
   aPt.stdev = stdev;
   out.push_back(aPt);

   delete myABA; 

   return 0;
}
//______________________________________________________________________________
TGraphErrors *GetTGraphErrors(std::vector<producedVariable_t> unser,std::vector<producedVariable_t> bcm){
   // produce a plot of the BCM rate (y axis) against the unser current (x axis)  

   const int NX = unser.size();
   const int NY = bcm.size();
   if(NX!=NY){
      std::cout << "[GetTGraphErrors]: ERROR! Unser NPTS != BCM NPTS!" << std::endl;
      exit(1); 
   }

   std::vector<double> x,ex,y,ey; 
   for(int i=0;i<NX;i++){
      x.push_back(unser[i].mean); 
      ex.push_back(unser[i].stdev); 
      y.push_back(bcm[i].mean); 
      ey.push_back(bcm[i].stdev); 
   }

   TGraphErrors *g = graph_df::GetTGraphErrors(x,ex,y,ey); 
   return g;
}
//______________________________________________________________________________
double myFitFunc(double *x,double *p){
   // linear fit
   double f = p[0] + p[1]*x[0];
   return f;
}
