// read in pedestal and fitted offset and slope (gain) files 
// and create calibration coefficient files 

#include <cstdlib>
#include <iostream>

#include "./src/Graph.cxx"
#include "./src/bcmUtilities.cxx"

int bcmBuildFiles(){
   
   int rc=0;

   // load the unser calibration result 
   char inpath_unser_calib[200]; 
   sprintf(inpath_unser_calib,"./output/12-07-21/unser-calib-results_0-150.csv"); 
 
   std::vector<calibCoeff_t> uns_cc;
   rc = bcm_util::LoadFittedOffsetGainData(inpath_unser_calib,uns_cc);

   // TODO: load offset and gain data for all other BCMs 
   // char cc_prefix[200]; 
   // sprintf(cc_prefix,"./output/"); 
   // char inpath_u1_cc[200],inpath_unew_cc[200]; 
   // char inpath_d1_cc[200],inpath_d3_cc[200],inpath_d10_cc[200],inpath_dnew_cc[200]; 
   // sprintf(inpath_u1_cc  ,"%s/u1-calib-results.csv"  ,cc_prefix); 
   // sprintf(inpath_unew_cc,"%s/unew-calib-results.csv",cc_prefix); 
   // sprintf(inpath_d1_cc  ,"%s/d1-calib-results.csv"  ,cc_prefix); 
   // sprintf(inpath_d3_cc  ,"%s/d3-calib-results.csv"  ,cc_prefix); 
   // sprintf(inpath_d10_cc ,"%s/d10-calib-results.csv" ,cc_prefix); 
   // sprintf(inpath_dnew_cc,"%s/dnew-calib-results.csv",cc_prefix); 

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

   std::vector<calibCoeff_t> ped,cc; 

   char inpath[200],outpath[200]; 

   int M=0;
   for(int i=0;i<N;i++){
      // get pedestal data for a given BCM 
      sprintf(inpath ,"%s/pedestal_%s.csv"   ,prefix_ped,bcm[i].c_str()); 
      rc = bcm_util::LoadPedestalData(inpath,ped);
      // build the *full* calibration coefficient struct
      // first we copy all pedestal info to the new cc vector 
      M = ped.size(); 
      for(int j=0;j<M;j++) cc.push_back(ped[j]); 
      // if it's the Unser, we need to apply the fitted result for offset and gain
      // note: the uns_cc has only a single entry in its vector!  
      if(bcm[i].compare("unser")==0){
	 for(int j=0;j<M;j++){
	    cc[j].offset    = uns_cc[0].offset;
	    cc[j].offsetErr = uns_cc[0].offsetErr;
	    cc[j].slope     = uns_cc[0].slope;
	    cc[j].slopeErr  = uns_cc[0].slopeErr;
	 }
      }
      // TODO: apply offsets and slopes/gains for all other BCMs 
      // print to files 
      sprintf(outpath,"%s/calib-coeff_%s.csv",prefix_cc,bcm[i].c_str()); 
      bcm_util::WriteToFile_cc(outpath,cc);
      // set up for next BCM 
      cc.clear();
      ped.clear(); 
   }  

   return 0;
}
