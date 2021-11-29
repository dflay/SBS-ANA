// compute BCM calibration coefficients

#include <cstdlib>
#include <iostream>
#include <vector>
#include <string> 

#include "./include/producedVariable.h"
#include "./src/bcmUtilities.cxx"

int bcmCalibrate(){

   std::vector<std::string> label,path;
   int rc = bcm_util::LoadConfigPaths("./input/bcm-calib.csv",label,path); 

   std::string on_path,off_path,out_path; 
   const int NF = label.size();
   for(int i=0;i<NF;i++){
      if(label[i].compare("beam_off")==0) off_path = path[i]; 
      if(label[i].compare("beam_on")==0)  on_path  = path[i]; 
      if(label[i].compare("outpath")==0)  out_path = path[i]; 
   }

   std::vector<producedVariable_t> off;
   rc = bcm_util::LoadProducedVariables(off_path.c_str(),off);
   if(rc!=0) return 1; 

   std::vector<producedVariable_t> on;
   rc = bcm_util::LoadProducedVariables(on_path.c_str(),on);
   if(rc!=0) return 1; 

   // compute difference for all variables 

   std::vector<std::string> DEV,BEAM_STATE; 
   std::vector<int> GRP; 
   std::vector<double> MU,SIG; 

   double arg=0,argErr=0; 
   producedVariable_t x; 
   std::vector<producedVariable_t> diff; 
   const int N = off.size();
   for(int i=0;i<N;i++){
      arg          = on[i].mean - off[i].mean; 
      argErr       = TMath::Sqrt( on[i].stdev*on[i].stdev + off[i].stdev*off[i].stdev );
      MU.push_back(arg); 
      SIG.push_back(argErr);
      DEV.push_back(on[i].dev);
      BEAM_STATE.push_back("DIFF"); 
      GRP.push_back(on[i].group); 
      // print 
      std::cout << Form("%s: on: %.3lf ± %.3lf, off: %.3lf ± %.3lf, on-off: %.3lf ± %.3lf",
                        on[i].dev.c_str(),on[i].mean,on[i].stdev,off[i].mean,off[i].stdev,arg,argErr) << std::endl; 
   }

   // write results to file
   const int NROW = DEV.size();
   const int NCOL = 5;
   CSVManager *csv = new CSVManager();
   csv->InitTable(NROW,NCOL);
   csv->SetColumn_str(0,DEV);
   csv->SetColumn_str(1,BEAM_STATE);
   csv->SetColumn<int>(2,GRP);
   csv->SetColumn<double>(3,MU);
   csv->SetColumn<double>(4,SIG);
   csv->SetHeader("dev,beam_state,group,mean,stdev");
   csv->WriteFile(out_path.c_str());
   delete csv;

   return 0;
}
