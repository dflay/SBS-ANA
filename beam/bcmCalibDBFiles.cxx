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
 
   char inpath[200],prefix[200],log_path[200],db_path_lhrs[200],db_path_sbs[200],msg[200]; 
   sprintf(prefix,"./output/%s",tag.c_str());
   sprintf(db_path_lhrs,"%s/db_LeftBCM.dat",prefix);
   sprintf(db_path_sbs ,"%s/db_sbsBCM.dat",prefix);
   sprintf(log_path    ,"%s/log/db.txt"   ,prefix);

   // load ALL results 
   std::vector<calibCoeff_t> cc;
   sprintf(inpath,"%s/result.csv",prefix);  
   rc = bcm_util::LoadCalibrationCoefficients(inpath,cc); 

   // also get the Unser 
   rc = bcm_util::LoadCalibrationCoefficients(unser_path.c_str(),cc); 

   char dev_lmsg[200],dev_smsg[200]; 
   char gain_msg[200],offs_msg[200],satq_msg[200];
   char sato_msg[200],curt_msg[200],curi_msg[200]; 

   double totOffset=0,totOffsetErr=0;
   const int NCC = cc.size();
   for(int i=0;i<NCC;i++){
      // DB expects a TOTAL offset -- that is pedestal + fitted offset
      // we construct it here 
      totOffset    = cc[i].pedestal + cc[i].offset;
      totOffsetErr = TMath::Sqrt( cc[i].pedestalErr*cc[i].pedestalErr + cc[i].offsetErr*cc[i].offsetErr ); 
      // sprintf(msg,"%s: pedestal = %.3lf, offset = %.3lf, total = %.3lf",cc[i].dev.c_str(),cc[i].pedestal,cc[i].offset,totOffset); 
      // util_df::LogMessage(log_path,msg,'a'); 
      if(i==0){
	 sprintf(dev_lmsg,"BCM_Names                   = Left.bcm.%s.current",cc[i].dev.c_str()); 
	 sprintf(dev_smsg,"BCM_Names                   = sbs.bcm.%s.current",cc[i].dev.c_str()); 
	 sprintf(gain_msg,"BCM_Gain                    = %.3lf",cc[i].slope); 
	 sprintf(offs_msg,"BCM_Offset                  = %.3lf",totOffset); 
	 sprintf(satq_msg,"BCM_SatQuadratic            = %.3lf",0.); 
	 sprintf(sato_msg,"BCM_SatOffset               = %.3lf",0.); 
	 sprintf(curt_msg,"BCM_Current_threshold       = %.3lf",0.); 
	 sprintf(curi_msg,"BCM_Current_threshold_index = %.3lf",0.); 
      }else{
	 sprintf(dev_lmsg ,"%s Left.bcm.%s.current"  ,dev_lmsg ,cc[i].dev.c_str()); 
	 sprintf(dev_smsg ,"%s sbs.bcm.%s.current"   ,dev_smsg ,cc[i].dev.c_str()); 
	 sprintf(gain_msg,"%s %.3lf",gain_msg,cc[i].slope); 
	 sprintf(offs_msg,"%s %.3lf",offs_msg,totOffset); 
	 sprintf(satq_msg,"%s %.3lf",satq_msg,0.); 
	 sprintf(sato_msg,"%s %.3lf",sato_msg,0.); 
	 sprintf(curt_msg,"%s %.3lf",curt_msg,0.); 
	 sprintf(curi_msg,"%s %.3lf",curi_msg,0.); 
      }
      // // make LHRS file
      // sprintf(varStr,"LeftBCM.%s.offset",cc[i].dev.c_str()); 
      // sprintf(msg,"%s = %.3lf",varStr,totOffset); 
      // util_df::LogMessage(db_path_lhrs,msg,'a');
      // sprintf(varStr,"LeftBCM.%s.gain",cc[i].dev.c_str()); 
      // sprintf(msg,"%s = %.3lf"  ,varStr,cc[i].slope); 
      // util_df::LogMessage(db_path_lhrs,msg,'a');
      // // make SBS file
      // sprintf(varStr,"sbsBCM.%s.offset",cc[i].dev.c_str()); 
      // sprintf(msg,"%s = %.3lf",varStr,totOffset); 
      // util_df::LogMessage(db_path_sbs,msg,'a');
      // sprintf(varStr,"sbsBCM.%s.gain",cc[i].dev.c_str()); 
      // sprintf(msg,"%s = %.3lf"  ,varStr,cc[i].slope); 
      // util_df::LogMessage(db_path_sbs,msg,'a');
   }

   std::string dev_lmsg_str = dev_lmsg;  
   std::string dev_smsg_str = dev_smsg;  
   std::string gain_msg_str = gain_msg;
   std::string offs_msg_str = offs_msg;
   std::string satq_msg_str = satq_msg;
   std::string sato_msg_str = sato_msg;
   std::string curt_msg_str = curt_msg;     
   std::string curi_msg_str = curi_msg;

   std::vector<std::string> MSG; 
   MSG.push_back(gain_msg_str); 
   MSG.push_back(offs_msg_str); 
   MSG.push_back(satq_msg_str); 
   MSG.push_back(sato_msg_str); 
   MSG.push_back(curt_msg_str); 
   MSG.push_back(curi_msg_str); 

   // now print to file
   std::string lhrs,sbs;
   lhrs = "LeftBCM." + dev_lmsg_str;
   sbs  = "sbsBCM."  + dev_lmsg_str;
   util_df::LogMessage(db_path_lhrs,lhrs.c_str(),'a'); 
   util_df::LogMessage(db_path_sbs ,sbs.c_str() ,'a'); 

   const int NS = MSG.size();
   for(int i=0;i<NS;i++){
      lhrs = "LeftBCM." + MSG[i];
      sbs  = "sbsBCM."  + MSG[i];
      util_df::LogMessage(db_path_lhrs,lhrs.c_str(),'a'); 
      util_df::LogMessage(db_path_sbs ,sbs.c_str() ,'a'); 
   }

   return 0;
}
