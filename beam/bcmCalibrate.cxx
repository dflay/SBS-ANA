// compute BCM calibration coefficients

#include <cstdlib>
#include <iostream>
#include <vector>
#include <string> 

#include "./include/producedVariable.h"
#include "./src/bcmUtilities.cxx"

int PrintToFile(const char *outpath,std::vector<producedVariable_t> data); 

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
   bool printToScreen = true;
   std::vector<producedVariable_t> diff;
   bcm_util::SubtractBaseline(on,off,diff,printToScreen);  

   rc = PrintToFile(out_path.c_str(),diff); 

   return rc;
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
   csv->WriteFile(out_path.c_str());

   delete csv;
   return 0;
}
