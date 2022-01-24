// read in pedestal and fitted offset and slope (gain) files 
// and create calibration coefficient files 

#include <cstdlib>
#include <iostream>

#include "./src/Graph.cxx"
#include "./src/JSONManager.cxx"
#include "./src/bcmUtilities.cxx"

int bcmBuildFiles(const char *confPath){
   
   int rc=0;

   JSONManager *jmgr = new JSONManager(confPath);
   std::string inpath_unser = jmgr->GetValueFromKey_str("unser-path"); 
   std::string inpath_bcm   = jmgr->GetValueFromKey_str("bcm-path");
   delete jmgr;  
 
   // std::vector<calibCoeff_t> uns_cc;
   // rc = bcm_util::LoadFittedOffsetGainData(inpath_unser.c_str(),uns_cc);

   // // load offset and gain data for all other BCMs 
   // char inpath_u1_cc[200],inpath_unew_cc[200]; 
   // char inpath_d1_cc[200],inpath_d3_cc[200],inpath_d10_cc[200],inpath_dnew_cc[200]; 
   // sprintf(inpath_u1_cc  ,"%s/u1-calib-results.csv"  ,inpath_bcm.c_str()); 
   // sprintf(inpath_unew_cc,"%s/unew-calib-results.csv",inpath_bcm.c_str()); 
   // sprintf(inpath_d1_cc  ,"%s/d1-calib-results.csv"  ,inpath_bcm.c_str()); 
   // sprintf(inpath_d3_cc  ,"%s/d3-calib-results.csv"  ,inpath_bcm.c_str()); 
   // sprintf(inpath_d10_cc ,"%s/d10-calib-results.csv" ,inpath_bcm.c_str()); 
   // sprintf(inpath_dnew_cc,"%s/dnew-calib-results.csv",inpath_bcm.c_str()); 

   // std::vector<calibCoeff_t> u1_cc,unew_cc;
   // std::vector<calibCoeff_t> d1_cc,d3_cc,d10_cc,dnew_cc;
   // rc = bcm_util::LoadFittedOffsetGainData(inpath_u1_cc  ,u1_cc);
   // rc = bcm_util::LoadFittedOffsetGainData(inpath_unew_cc,unew_cc);
   // rc = bcm_util::LoadFittedOffsetGainData(inpath_d1_cc  ,d1_cc);
   // rc = bcm_util::LoadFittedOffsetGainData(inpath_d3_cc  ,d3_cc);
   // rc = bcm_util::LoadFittedOffsetGainData(inpath_d10_cc ,d10_cc);
   // rc = bcm_util::LoadFittedOffsetGainData(inpath_dnew_cc,dnew_cc);

   // load pedestal results 
   const int N = 7; 
   std::string bcm[N] = {"unser","u1","unew","d1","d3","d10","dnew"};
 
   char prefix_ped[200],prefix_cc[200]; 
   sprintf(prefix_ped,"./output/pedestal");
   sprintf(prefix_cc ,"./output/calib-coeff");

   std::vector<calibCoeff_t> ped,og,cc; 

   char inpath[200],outpath[200]; 

   int M=0;
   for(int i=0;i<N;i++){
      // get pedestal data for a given BCM 
      sprintf(inpath ,"%s/pedestal_%s.csv"   ,prefix_ped,bcm[i].c_str()); 
      rc = bcm_util::LoadPedestalData(inpath,ped);
      // build the *full* calibration coefficient struct
      // first we copy all pedestal info to the new cc vector 
      M = ped.size();  // determine number of pedestal periods 
      for(int j=0;j<M;j++) cc.push_back(ped[j]);
      // now load the fitted offsets and gains
      if(bcm[i].compare("unser")==0){
	 sprintf(inpath,"%s",inpath_unser.c_str());
      }else{
	 sprintf(inpath,"%s/%s-calib-results.csv",inpath_bcm.c_str(),bcm[i].c_str());
      } 
      rc = bcm_util::LoadFittedOffsetGainData(inpath,og); 
      for(int j=0;j<M;j++){
	 cc[j].offset    = og[0].offset;
	 cc[j].offsetErr = og[0].offsetErr;
	 cc[j].slope     = og[0].slope;
	 cc[j].slopeErr  = og[0].slopeErr;
      }
      // // if it's the Unser, we need to apply the fitted result for offset and gain
      // // note: the uns_cc has only a single entry in its vector!  
      // if(bcm[i].compare("unser")==0){
      //    for(int j=0;j<M;j++){
      //       cc[j].offset    = uns_cc[0].offset;
      //       cc[j].offsetErr = uns_cc[0].offsetErr;
      //       cc[j].slope     = uns_cc[0].slope;
      //       cc[j].slopeErr  = uns_cc[0].slopeErr;
      //    }
      // }else if(bcm[i].compare("u1")==0){
      //    for(int j=0;j<M;j++){
      //       cc[j].offset    = u1[0].offset;
      //       cc[j].offsetErr = u1[0].offsetErr;
      //       cc[j].slope     = u1[0].slope;
      //       cc[j].slopeErr  = u1[0].slopeErr;
      //    }
      // }else if(bcm[i].compare("unew")==0){
      //    for(int j=0;j<M;j++){
      //       cc[j].offset    = unew[0].offset;
      //       cc[j].offsetErr = unew[0].offsetErr;
      //       cc[j].slope     = unew[0].slope;
      //       cc[j].slopeErr  = unew[0].slopeErr;
      //    }
      // }else if(bcm[i].compare("d1")==0){
      //    for(int j=0;j<M;j++){
      //       cc[j].offset    = d1[0].offset;
      //       cc[j].offsetErr = d1[0].offsetErr;
      //       cc[j].slope     = d1[0].slope;
      //       cc[j].slopeErr  = d1[0].slopeErr;
      //    }
      // }else if(bcm[i].compare("d3")==0){
      //    for(int j=0;j<M;j++){
      //       cc[j].offset    = d3[0].offset;
      //       cc[j].offsetErr = d3[0].offsetErr;
      //       cc[j].slope     = d3[0].slope;
      //       cc[j].slopeErr  = d3[0].slopeErr;
      //    }
      // }else if(bcm[i].compare("d10")==0){
      //    for(int j=0;j<M;j++){
      //       cc[j].offset    = d10[0].offset;
      //       cc[j].offsetErr = d10[0].offsetErr;
      //       cc[j].slope     = d10[0].slope;
      //       cc[j].slopeErr  = d10[0].slopeErr;
      //    }
      // }else if(bcm[i].compare("dnew")==0){
      //    for(int j=0;j<M;j++){
      //       cc[j].offset    = dnew[0].offset;
      //       cc[j].offsetErr = dnew[0].offsetErr;
      //       cc[j].slope     = dnew[0].slope;
      //       cc[j].slopeErr  = dnew[0].slopeErr;
      //    }
      // }
      // print to files 
      sprintf(outpath,"%s/01-18-22/calib-coeff_%s.csv",prefix_cc,bcm[i].c_str()); 
      bcm_util::WriteToFile_cc(outpath,cc);
      // set up for next BCM 
      cc.clear();
      og.clear();
      ped.clear(); 
   }  

   return 0;
}
//______________________________________________________________________________
