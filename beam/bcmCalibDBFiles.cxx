// read in pedestal and fitted offset and slope (gain) files 
// and create calibration coefficient files
// for use in the analyzer 

#include <cstdlib>
#include <iostream>

#include "TTree.h"
#include "TFile.h"
#include "TH1F.h"
#include "TStyle.h"
#include "TPad.h"
#include "TLine.h"

#include "./include/scalerData.h"
#include "./include/epicsData.h"
#include "./include/producedVariable.h"
#include "./include/calibCoeff.h"
#include "./include/codaRun.h"
#include "./src/ABA.cxx"
#include "./src/Graph.cxx"
#include "./src/CSVManager.cxx"
#include "./src/JSONManager.cxx"
#include "./src/BCMManager.cxx"
#include "./src/Utilities.cxx"
#include "./src/bcmUtilities.cxx"

int bcmCalibDBFiles(const char *confPath){
   
   int rc=0;

   JSONManager *jmgr = new JSONManager(confPath);
   std::string tag        = jmgr->GetValueFromKey_str("tag");
   std::string unser_path = jmgr->GetValueFromKey_str("unser-cc-path");   
   delete jmgr;  
 
   char inpath[200],prefix[200],log_path[200],db_path_left[200],db_path_sbs[200],msg[200]; 
   sprintf(prefix,"./output/%s",tag.c_str());
   sprintf(db_path_left,"%s/db_LeftBCM.dat",prefix);
   sprintf(db_path_sbs ,"%s/db_sbsBCM.dat",prefix);
   sprintf(log_path    ,"%s/log/db.txt"   ,prefix);

   // load ALL results 
   std::vector<calibCoeff_t> cc;
   sprintf(inpath,"%s/result.csv",prefix);  
   rc = bcm_util::LoadCalibrationCoefficients(inpath,cc); 

   // also get the Unser 
   rc = bcm_util::LoadCalibrationCoefficients(unser_path.c_str(),cc); 

   char varStr[200]; 

   double totOffset=0,totOffsetErr=0;
   const int NCC = cc.size();
   for(int i=0;i<NCC;i++){
      // DB expects a TOTAL offset -- that is pedestal + fitted offset
      // we construct it here 
      totOffset    = cc[i].pedestal + cc[i].offset;
      totOffsetErr = TMath::Sqrt( cc[i].pedestalErr*cc[i].pedestalErr + cc[i].offsetErr*cc[i].offsetErr ); 
      sprintf(msg,"%s: pedestal = %.3lf, offset = %.3lf, total = %.3lf",cc[i].dev.c_str(),cc[i].pedestal,cc[i].offset,totOffset); 
      util_df::LogMessage(log_path,msg,'a'); 
      // make LHRS file
      sprintf(varStr,"LeftBCM.%s.offset",cc[i].dev.c_str()); 
      sprintf(msg,"%s = %.3lf",varStr,totOffset); 
      util_df::LogMessage(db_path_left,msg,'a');
      sprintf(varStr,"LeftBCM.%s.gain",cc[i].dev.c_str()); 
      sprintf(msg,"%s = %.3lf"  ,varStr,cc[i].slope); 
      util_df::LogMessage(db_path_left,msg,'a');
      // make SBS file
      sprintf(varStr,"sbsBCM.%s.offset",cc[i].dev.c_str()); 
      sprintf(msg,"%s = %.3lf",varStr,totOffset); 
      util_df::LogMessage(db_path_sbs,msg,'a');
      sprintf(varStr,"sbsBCM.%s.gain",cc[i].dev.c_str()); 
      sprintf(msg,"%s = %.3lf"  ,varStr,cc[i].slope); 
      util_df::LogMessage(db_path_sbs,msg,'a');
   }

   return 0;
}
