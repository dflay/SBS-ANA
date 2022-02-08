#include "../include/cutUtilities.h"
namespace cut_util {
   // //______________________________________________________________________________
   // int LoadCuts_bcmCalib_json(const char *inpath,std::vector<std::string> key,std::vector< std::vector<cut_t> > &data){
   //    // load cuts into a JSON object first, then populate the vector of type cut_t
   //    // generally, this is a vector of cuts on a single x variable (cut-var), for a given 
   //    // y variable (dev).  The key "num-cuts" defines the number of cut pairs (min,max) 
   //    // to apply to the data   
   //    // taylored to BCM calibration analysis. Might have a lot of keys (for many beam currents)  
   //    JSONManager *jmgr = new JSONManager();
   //    int rc = jmgr->ReadFile(inpath);
   //    if(rc!=0){
   //       delete jmgr;
   //       return 1;
   //    } 

   //    std::string dev     = jmgr->GetValueFromKey_str("dev");
   //    std::string cut_var = jmgr->GetValueFromKey_str("cut-var");
   //   
   //    int NC = jmgr->GetValueFromKey<int>("ncycles"); 

   //    cut_t aCut;
   //    int NK = key.size();
   //    std::vector<double> min,max; 
   //    std::vector<cut_t> cut; 
   //    for(int i=0;i<NK;i++){ 
   //       // this is for the ith beam current 
   //       // beam off 
   //       jmgr->GetVectorFromSubKey<double>(key[i],"min-off",NC,min);  
   //       jmgr->GetVectorFromSubKey<double>(key[i],"max-off",NC,max); 
   //       // each beam current has a set of beam-on and and beam-off cuts 
   //       for(int j=0;j<NC;j++){
   //          aCut.dev     = dev; 
   //          aCut.cut_var = cut_var;
   //          aCut.low     = min[j];  
   //          aCut.high    = max[j]; 
   //          cut.push_back(aCut);  
   //       }
   //       data.push_back(cut); 
   //       // set up for next beam current 
   //       min.clear();
   //       max.clear();
   //       cut.clear();
   //    } 

   //    delete jmgr;  
   //    return 0;
   // }
   //______________________________________________________________________________
   int LoadCuts_json(const char *inpath,std::vector<cut_t> &data){
      // load cuts into a JSON object first, then populate the vector of type cut_t
      // generally, this is a vector of cuts on a single x variable (cut-var), for a given 
      // y variable (dev).  The key "num-cuts" defines the number of cut pairs (min,max) 
      // to apply to the data   
      JSONManager *jmgr = new JSONManager();
      int rc = jmgr->ReadFile(inpath);
      if(rc!=0){
	 delete jmgr;
	 return 1;
      } 

      std::string dev     = jmgr->GetValueFromKey_str("dev");
      std::string cut_var = jmgr->GetValueFromKey_str("cut-var");
     
      int NC = jmgr->GetValueFromKey<int>("num-cuts"); 
 
      std::vector<double> min,max; 
      jmgr->GetVectorFromKey<double>("min",NC,min);  
      jmgr->GetVectorFromKey<double>("max",NC,max); 

      delete jmgr;  

      cut_t pt;
      for(int i=0;i<NC;i++){
         pt.dev     = dev; 
	 pt.cut_var = cut_var;
         pt.low     = min[i];  
         pt.high    = max[i]; 
	 data.push_back(pt);  
      }
      return 0;
   }
   //______________________________________________________________________________
   int LoadCuts_epics_json(const char *inpath,std::vector<cut_t> &data){
      // load cuts into a JSON object first, then populate the vector of type cut_t
      // generally, this is a vector of cuts on a single x variable (cut-var), for a given 
      // y variable (dev).  The key "num-cuts" defines the number of cut pairs (min,max) 
      // to apply to the data   
      JSONManager *jmgr = new JSONManager();
      int rc = jmgr->ReadFile(inpath);
      if(rc!=0){
	 delete jmgr;
	 return 1;
      } 

      std::string dev     = jmgr->GetValueFromKey_str("dev-epics");
      std::string cut_var = jmgr->GetValueFromKey_str("cut-var-epics");
     
      int NC = jmgr->GetValueFromKey<int>("num-cuts-epics"); 
 
      std::vector<double> min,max; 
      jmgr->GetVectorFromKey<double>("min-epics",NC,min);  
      jmgr->GetVectorFromKey<double>("max-epics",NC,max); 

      delete jmgr;  

      // std::cout << dev << std::endl; 
      // std::cout << cut_var << std::endl;
      // std::cout << NC << std::endl;
 
      cut_t pt;
      for(int i=0;i<NC;i++){
         pt.dev     = dev; 
	 pt.cut_var = cut_var;
         pt.low     = min[i];  
         pt.high    = max[i]; 
	 data.push_back(pt);  
      }
      return 0;
   } 
   //______________________________________________________________________________
   int LoadCuts(const char *inpath,std::vector<cut_t> &data){
      // load cuts from a file into a vector of type cut_t  
      CSVManager *csv = new CSVManager();
      int rc = csv->ReadFile(inpath,true);
      if(rc!=0){
	 delete csv;
	 return 1;
      }

      std::vector<std::string> arm,dev,beam_state;
      csv->GetColumn_byName_str("arm"       ,arm);
      csv->GetColumn_byName_str("dev"       ,dev);
      csv->GetColumn_byName_str("beam_state",beam_state);

      std::vector<double> low,high;
      csv->GetColumn_byName<double>("cut_low" ,low);
      csv->GetColumn_byName<double>("cut_high",high);

      std::vector<int> grp;
      csv->GetColumn_byName<int>("group",grp);

      cut_t aCut;
      const int N = dev.size();
      for(int i=0;i<N;i++){
         aCut.arm        = arm[i];
	 aCut.dev        = dev[i];
	 aCut.beam_state = beam_state[i];
	 aCut.low        = low[i];
	 aCut.high       = high[i];
	 aCut.group      = grp[i];
	 data.push_back(aCut);
      }

      // delete CSV manager 
      delete csv;
  
      return 0;
   }
   //______________________________________________________________________________
   int ApplyCuts(std::vector<cut_t> cutList,std::vector<scalerData_t> in,std::vector<scalerData_t> &out){
      // apply cuts event by event according to a list of cuts
      // input: 
      // - cutList: a list of cuts with a min and max range; can be different BCM variables 
      // - in: input scaler data 
      // output: 
      // - out: scaler data that passed the cuts   
      const int NC  = cutList.size();
      const int NEV = in.size(); 

      int fail=0;
      bool pass=false,passAll=false;

      for(int i=0;i<NEV;i++){
	 // loop over cuts 
	 for(int j=0;j<NC;j++){
	    pass = PassCut(cutList[j],in[i]);
	    if(pass==false) fail++; 
	 }
	 // check to see if all cuts passed
         if(fail==0){
	    passAll = true;
         }else{
	    passAll = false;
         }
         // if we passed all cuts, fill the event 
         if(passAll) out.push_back(in[i]);
	 // reset for next event  
	 fail = 0;
	 pass = false;
	 passAll = false; 
      } 
      return 0;
   }
   //______________________________________________________________________________
   bool PassCut(cut_t cut,scalerData_t event){
      // check to see if the event data passes the cut
      // based on the variable name in the cut struct, get the associated 
      double val = event.getValue(cut.dev); 
      // check if the event passes the cut  
      bool pass = false;
      if( (val>cut.low)&&(val<cut.high) ) pass = true;
      return pass; 
   }
   //______________________________________________________________________________
   int ApplyCuts_alt(std::vector<cut_t> cutList,std::vector<scalerData_t> in,std::vector<scalerData_t> &out){
      // apply cuts event by event according to a list of cuts
      // use for ONE BCM variable as a function of event number (or other good x axis)
      // input: 
      // - cutList: a list of cuts with a min and max range. Must define the variable cut_var in cut struct 
      // - in: input scaler data 
      // output: 
      // - out: scaler data that passed the cuts   
      const int NC  = cutList.size();
      const int NEV = in.size(); 

      int fail=0,success=0;
      bool pass=false,passAll=false;

      for(int i=0;i<NEV;i++){
	 // loop over cuts 
	 for(int j=0;j<NC;j++){
	    pass = PassCut_alt(cutList[j],in[i]);
	    if(pass==true){
	       success++;
            } 
	 }
	 // // check to see if all cuts passed
         // if(fail==0){
	 //    passAll = true;
         // }else{
	 //    passAll = false;
         // }
         // if we passed at least one cut, fill the output vector 
         // (usually exactly one success over all cuts)   
         if(success>0) out.push_back(in[i]);
	 // reset for next event  
	 fail = 0;
	 pass = false;
	 success = 0;
	 // passAll = false; 
      } 
      return 0;
   }
   //______________________________________________________________________________
   bool PassCut_alt(cut_t cut,scalerData_t event){
      // check to see if the event data passes the cut
      // based on the variable name in the cut struct, get the associated value 

      // effectively no cut, pass the event
      if(cut.low==cut.high) return true;

      double val = event.getValue(cut.cut_var); 
      // check if the event passes the cut  
      bool pass = false;
      if( (val>cut.low)&&(val<cut.high) ) pass = true;
      return pass; 
   }
   //______________________________________________________________________________
   int ApplyCuts_alt(std::vector<cut_t> cutList,std::vector<epicsData_t> in,std::vector<epicsData_t> &out){
      // apply cuts event by event according to a list of cuts
      // use for ONE BCM variable as a function of event number (or other good x axis)
      // input: 
      // - cutList: a list of cuts with a min and max range. Must define the variable cut_var in cut struct 
      // - in: input scaler data 
      // output: 
      // - out: scaler data that passed the cuts   
      const int NC  = cutList.size();
      const int NEV = in.size(); 

      int fail=0,success=0;
      bool pass=false,passAll=false;

      for(int i=0;i<NEV;i++){
	 // loop over cuts 
	 for(int j=0;j<NC;j++){
	    pass = PassCut_alt(cutList[j],in[i]);
	    if(pass==true){
	       success++;
            } 
	 }
	 // // check to see if all cuts passed
         // if(fail==0){
	 //    passAll = true;
         // }else{
	 //    passAll = false;
         // }
         // if we passed at least one cut, fill the output vector 
         // (usually exactly one success over all cuts)   
         if(success>0) out.push_back(in[i]);
	 // reset for next event  
	 fail = 0;
	 pass = false;
	 success = 0;
	 // passAll = false; 
      } 
      return 0;
   }
   //______________________________________________________________________________
   bool PassCut_alt(cut_t cut,epicsData_t event){
      // check to see if the event data passes the cut
      // based on the variable name in the cut struct, get the associated value 

      // effectively no cut, pass the event
      if(cut.low==cut.high) return true; 

      double val = event.getValue(cut.cut_var); 
      // check if the event passes the cut  
      bool pass = false;
      if( (val>cut.low)&&(val<cut.high) ) pass = true;
      return pass; 
   }
   //______________________________________________________________________________
   int ApplyCut(std::string var,double lo,double hi,std::vector<scalerData_t> in,std::vector<scalerData_t> &out){
      // apply a cut to variable "var" with the bounds lo and hi
      // select the entire data struct of type scalerData_t that passes the cut 
      double val=0;
      const int N = in.size();
      for(int i=0;i<N;i++){
	 if(var.compare("run")==0)            val = in[i].runNumber; 
	 if(var.compare("event")==0)          val = in[i].event; 
	 if(var.compare("time")==0)           val = in[i].time; 
	 if(var.compare("unser.rate")==0)     val = in[i].unserRate; 
	 if(var.compare("unser.cnt")==0)      val = in[i].unserCounts; 
	 if(var.compare("unser.current")==0)  val = in[i].unserCurrent; 
	 if(var.compare("u1.rate")==0)        val = in[i].u1Rate; 
	 if(var.compare("u1.cnt")==0)         val = in[i].u1Counts; 
	 if(var.compare("u1.current")==0)     val = in[i].u1Current; 
	 if(var.compare("unew.rate")==0)      val = in[i].unewRate; 
	 if(var.compare("unew.cnt")==0)       val = in[i].unewCounts; 
	 if(var.compare("unew.current")==0)   val = in[i].unewCurrent; 
	 if(var.compare("d1.rate")==0)        val = in[i].d1Rate; 
	 if(var.compare("d1.cnt")==0)         val = in[i].d1Counts; 
	 if(var.compare("d1.current")==0)     val = in[i].d1Current; 
	 if(var.compare("d3.rate")==0)        val = in[i].d3Rate; 
	 if(var.compare("d3.cnt")==0)         val = in[i].d3Counts; 
	 if(var.compare("d3.current")==0)     val = in[i].d3Current; 
	 if(var.compare("d10.rate")==0)       val = in[i].d10Rate; 
	 if(var.compare("d10.cnt")==0)        val = in[i].d10Counts; 
	 if(var.compare("d10.current")==0)    val = in[i].d10Current; 
	 if(var.compare("dnew.rate")==0)      val = in[i].dnewRate; 
	 if(var.compare("dnew.cnt")==0)       val = in[i].dnewCounts; 
	 if(var.compare("dnew.current")==0)   val = in[i].dnewCurrent;
	 // apply the cut 
         if( (val>lo)&&(val<hi) ){
	    out.push_back(in[i]); 
         } 
      }
      return 0;
   }
   //______________________________________________________________________________
   int ApplyCuts(double cutLo,double cutHi,std::vector<double> x,std::vector<double> y,std::vector<double> &out){
      // cuts are applied to the x variable. out has data that passed the cut 
      const int N = x.size();
      for(int i=0;i<N;i++){
	 if(x[i]>cutLo&&x[i]<cutHi) out.push_back(y[i]);
      }
      return 0;
   }
   //______________________________________________________________________________
   int GetStatsWithCuts(std::vector<double> x,std::vector<double> y,
	 double cutLo,double cutHi,double &mean,double &stdev){
      // cuts are applied to the x variable. if true, compute stats on y 
      std::vector<double> Y;
      const int N = x.size();
      for(int i=0;i<N;i++){
	 if(x[i]>cutLo&&x[i]<cutHi) Y.push_back(y[i]);
      }
      mean  = math_df::GetMean<double>(Y);
      stdev = math_df::GetStandardDeviation<double>(Y);
      return 0;
   }
}
