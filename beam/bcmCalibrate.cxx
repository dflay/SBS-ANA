// compute BCM calibration coefficients

#include <cstdlib>
#include <iostream>
#include <vector>
#include <string> 

#include "./include/calibCoeff.h"
#include "./include/producedVariable.h"
#include "./src/bcmUtilities.cxx"
#include "./src/Graph.cxx"

bool gIsDebug = false;

double myFitFunc(double *x,double *p); 

int PrintToFile(const char *outpath,std::vector<calibCoeff_t> data); 
int PrintToFile(const char *outpath,std::vector<producedVariable_t> data); 
int GetUnserCurrent(std::vector<producedVariable_t> data,std::vector<double> cc,
                    std::vector<double> &x,std::vector<double> &dx); 

TGraphErrors *GetTGraphErrors(std::string xAxis,std::string yAxis,std::vector<producedVariable_t> data); 

int bcmCalibrate(){

   int rc=0;
   std::vector<std::string> label,path;
   rc = bcm_util::LoadConfigPaths("./input/bcm-calib.csv",label,path); 

   std::string on_path,off_path,out_path,e_path,out_path_cc; 
   const int NF = label.size();
   for(int i=0;i<NF;i++){
      if(label[i].compare("beam_off")==0)   off_path    = path[i]; 
      if(label[i].compare("beam_on")==0)    on_path     = path[i]; 
      if(label[i].compare("epics")==0)      e_path      = path[i]; 
      if(label[i].compare("outpath")==0)    out_path    = path[i]; 
      if(label[i].compare("outpath_cc")==0) out_path_cc = path[i]; 
   }

   std::vector<producedVariable_t> off;
   rc = bcm_util::LoadProducedVariables(off_path.c_str(),off);
   if(rc!=0) return 1; 

   std::vector<producedVariable_t> on;
   rc = bcm_util::LoadProducedVariables(on_path.c_str(),on);
   if(rc!=0) return 1; 

   std::vector<producedVariable_t> epics; 
   rc = bcm_util::LoadProducedVariables(e_path.c_str(),epics);
   if(rc!=0) return 1; 

   // compute difference for all variables
   bool printToScreen = true;
   std::vector<producedVariable_t> diff;
   bcm_util::SubtractBaseline(on,off,diff,printToScreen);  
   rc = PrintToFile(out_path.c_str(),diff);
 
   // now append the epics data to the diff vector for processing below 
   const int NE = epics.size();
   for(int i=0;i<NE;i++){
      std::cout << "Adding: " << epics[i].dev << std::endl;
      bcm_util::Print(epics[i]); 
      diff.push_back(epics[i]); 
   }
 
   // start extracting calibration coefficients 

   // set up fit function 
   const int npar = 2; 
   double par[npar] = {0,0};
   double min = 0; 
   double max = 100; // in uA   
   TF1 *myFit = new TF1("myFit",myFitFunc,min,max,npar);
   for(int i=0;i<npar;i++) myFit->SetParameter(i,0);
 
   // get EPICS beam current value, and Unser values and create a plot  
   TGraphErrors *gUnser_cc = GetTGraphErrors("IBC1H04CRCUR2","unser",diff);  
   graph_df::SetParameters(gUnser_cc,20,kBlack); 

   TCanvas *c1 = new TCanvas("c1","Unser vs IBC1H04CRCUR2",1200,800);
 
   c1->cd();
   gUnser_cc->Draw("ap");
   graph_df::SetLabels(gUnser_cc,"Unser Calibration","IBC1H04CRCUR2 [#muA]","Unser [Hz]"); 
   gUnser_cc->Draw("ap");
   gUnser_cc->Fit("myFit","Q"); 
   c1->Update(); 

   // extract fit parameters 
   double offset = myFit->GetParameter(0); 
   double slope  = myFit->GetParameter(1);

   std::cout << "====== FIT RESULTS ======" << std::endl;
   std::cout << Form("dev    = unser")              << std::endl;
   std::cout << Form("offset = %.3lf Hz"   ,offset) << std::endl; 
   std::cout << Form("slope  = %.3lf Hz/uA",slope ) << std::endl;
   std::cout << "=========================" << std::endl; 

   std::vector<double> unser_cc;
   unser_cc.push_back(offset); 
   unser_cc.push_back(slope); 

   // plot BCMs against the Unser [current].  The extracted slopes are used in the calibration coefficients  
   // get the unser current 
   std::vector<double> ux,udx; 
   rc = GetUnserCurrent(diff,unser_cc,ux,udx);  
   
   // make all BCM plots
   std::vector<double> y,dy; 
   std::vector<std::string> BCMName;
   BCMName.push_back("u1"); 
   BCMName.push_back("unew"); 
   BCMName.push_back("dnew"); 
   BCMName.push_back("d1"); 
   BCMName.push_back("d3"); 
   BCMName.push_back("d10"); 
   const int NB = BCMName.size();;  
   TGraphErrors **g = new TGraphErrors*[NB]; 
   for(int i=0;i<NB;i++){
      bcm_util::GetData(BCMName[i],diff,y,dy);
      g[i] = graph_df::GetTGraphErrors(ux,y,dy); 
      graph_df::SetParameters(g[i],20,kBlack);  
      // set up for next one 
      y.clear();
      dy.clear();  
   }

   TCanvas *c2 = new TCanvas("c2","BCM Calibration",1400,800); 
   c2->Divide(3,2); 

   TString Title,yAxisTitle;
   TString xAxisTitle = Form("Unser Current [#muA]");

   calibCoeff_t cc; 
   std::vector<calibCoeff_t> CC; 

   std::cout << "===== FIT RESULTS =====" << std::endl;
   for(int i=0;i<NB;i++){
      Title      = Form("%s",BCMName[i].c_str());
      yAxisTitle = Form("%s [Hz]",BCMName[i].c_str());
      c2->cd(i+1);
      g[i]->Draw("ap"); 
      graph_df::SetLabels(g[i],Title,xAxisTitle,yAxisTitle); 
      // graph_df::SetLabelSizes(g[i],0.05,0.06); 
      g[i]->Draw("ap");
      g[i]->Fit("myFit","Q");
      c2->Update(); 
      // get fit parameters 
      offset = myFit->GetParameter(0);  
      slope  = myFit->GetParameter(1);
      cc.dev = BCMName[i]; 
      cc.offset = offset; 
      cc.slope  = slope;
      CC.push_back(cc); 
      std::cout << Form("dev = %s, offset = %.3lf Hz, slope = %.3lf Hz/uA",BCMName[i].c_str(),offset,slope) << std::endl;
   }
   std::cout << "=======================" << std::endl;

   rc = WriteToFile(out_path_cc.c_str(),CC);

   return rc;
}
//______________________________________________________________________________
int GetUnserCurrent(std::vector<producedVariable_t> data,std::vector<double> cc,
                    std::vector<double> &x,std::vector<double> &dx){
   // get unser data out of the full list of beam-off subtracted data
   std::vector<producedVariable_t> X;
   const int N = data.size();
   for(int i=0;i<N;i++){
      if(data[i].dev.compare("unser")==0) X.push_back(data[i]);  
   } 
   // now compute the unser current [in uA]  
   const int NX = X.size();
   if(NX==0){
      std::cout << "[bcmCalibrate::GetUnserCurrent]: No data!" << std::endl;
      return 1;
   }
   double offset = cc[0]; 
   double slope  = cc[1]; 
   double arg=0,argErr=0;
   for(int i=0;i<NX;i++){
      arg    = (X[i].mean-offset)/slope;
      argErr = X[i].stdev/slope;
      x.push_back(arg);   
      dx.push_back(argErr);   
   } 
   return 0;
}
//______________________________________________________________________________
TGraphErrors *GetTGraphErrors(std::string xAxis,std::string yAxis,std::vector<producedVariable_t> data){
   // get a plot from the list of produced variables 
   std::vector<producedVariable_t> X,Y; 

   const int N = data.size();
   for(int i=0;i<N;i++){
      // find the x and y axes  
      if(xAxis.compare(data[i].dev)==0){
	 X.push_back(data[i]);  
      }
      if(yAxis.compare(data[i].dev)==0){
	 Y.push_back(data[i]);  
      }
   }

   const int NX = X.size(); 
   const int NY = Y.size(); 

   if(gIsDebug){
      std::cout << "x axis data: " << std::endl; 
      for(int i=0;i<NX;i++) bcm_util::Print(X[i]);  
   } 

   // store data to useful vectors for plotting 
   std::vector<double> x,ex; 
   for(int i=0;i<NX;i++){
      x.push_back(X[i].mean); 
      ex.push_back(X[i].stdev); 
   }

   if(gIsDebug){
      std::cout << "y axis data: " << std::endl; 
      for(int i=0;i<NY;i++) bcm_util::Print(Y[i]);   
   } 

   // store data to useful vectors for plotting 
   std::vector<double> y,ey; 
   for(int i=0;i<NY;i++){
      y.push_back(Y[i].mean); 
      ey.push_back(Y[i].stdev); 
   }

   TGraphErrors *g = graph_df::GetTGraphErrors(x,y,ey); 
   return g;

}
//______________________________________________________________________________
int PrintToFile(const char *outpath,std::vector<calibCoeff_t> data){

  std::vector<std::string> dev;
  std::vector<double> offset,offsetErr,slope,slopeErr;
  const int N = data.size();
  for(int i=0;i<N;i++){
     dev.push_back(data[i].dev); 
     offset.push_back(data[i].offset); 
     offsetErr.push_back(data[i].offsetErr); 
     slope.push_back(data[i].slope);  
     slopeErr.push_back(data[i].slopeErr); 
  }

  std::string header = "dev,offset(Hz),offsetErr(Hz),slope(Hz/uA),slopeErr(Hz/uA)"; 

  const int NROW = dev.size();
  const int NCOL = 5;  
  CSVManager *csv = new CSVManager();
  csv->InitTable(NROW,NCOL);
  csv->SetColumn_str(0,dev); 
  csv->SetColumn<double>(1,offset); 
  csv->SetColumn<double>(2,offsetErr); 
  csv->SetColumn<double>(3,slope); 
  csv->SetColumn<double>(4,slopeErr);
  csv->SetHeader(header);
  csv->WriteFile(outpath); 

  delete csv;  

  return 0;
}
//______________________________________________________________________________
int PrintToFile(const char *outpath,std::vector<producedVariable_t> data){

   std::vector<std::string> DEV,BEAM_STATE; 
   std::vector<int> GRP; 
   std::vector<double> MU,SIG; 

   // write results to file
   const int N = data.size();
   for(int i=0;i<N;i++){
      MU.push_back(data[i].mean); 
      SIG.push_back(data[i].stdev);
      DEV.push_back(data[i].dev);
      BEAM_STATE.push_back(data[i].beam_state); 
      GRP.push_back(data[i].group); 
   }

   std::string header = "dev,beam_state,group,mean,stdev"; 

   const int NROW = DEV.size();
   const int NCOL = 5;
   CSVManager *csv = new CSVManager();
   csv->InitTable(NROW,NCOL);
   csv->SetColumn_str(0,DEV);
   csv->SetColumn_str(1,BEAM_STATE);
   csv->SetColumn<int>(2,GRP);
   csv->SetColumn<double>(3,MU);
   csv->SetColumn<double>(4,SIG);
   csv->SetHeader(header);
   csv->WriteFile(outpath);

   delete csv;
   return 0;
}
//______________________________________________________________________________
double myFitFunc(double *x,double *p){
   // linear fit
   double f = p[0] + p[1]*x[0]; 
   return f; 
}
