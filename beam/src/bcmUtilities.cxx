#include "../include/bcmUtilities.h"
#include "CSVManager.cxx"
#include "Math.cxx"
//______________________________________________________________________________
namespace bcm_util {
   //______________________________________________________________________________
   int LoadProducedVariables(const char *inpath,std::vector<producedVariable_t> &data){
      // load run list from a file 
      CSVManager *csv = new CSVManager();
      int rc = csv->ReadFile(inpath,true);
      if(rc!=0){
	 delete csv;
	 return 1;
      }
   
      std::vector<std::string> dev,beam_state;
      csv->GetColumn_byIndex_str(0,dev); 
      csv->GetColumn_byIndex_str(1,beam_state); 

      std::vector<int> group;
      csv->GetColumn_byIndex<int>(2,group);
 
      std::vector<double> mean,stdev;  
      csv->GetColumn_byIndex<double>(3,mean);
      csv->GetColumn_byIndex<double>(4,stdev);
     
      const int N = dev.size(); 
      producedVariable_t var; 
      for(int i=0;i<N;i++){
	 var.dev        = dev[i];
         var.beam_state = beam_state[i];  
         var.group      = group[i];  
         var.mean       = mean[i];  
         var.stdev      = stdev[i]; 
	 data.push_back(var);  
      }
 
      delete csv; 
      return 0;  
   }
   //______________________________________________________________________________
   int LoadConfigPaths(const char *inpath,std::vector<std::string> &label,std::vector<std::string> &path){
      // load run list from a file 
      CSVManager *csv = new CSVManager();
      int rc = csv->ReadFile(inpath);
      if(rc!=0){
	 delete csv;
	 return 1;
      }
     
      csv->GetColumn_byIndex_str(0,label);
      csv->GetColumn_byIndex_str(1,path );

      delete csv; 
      return 0;  
   }
   //______________________________________________________________________________
   int LoadRuns(const char *inpath,TString &prefix,std::vector<int> &run){
      // load run list from a file 
      CSVManager *csv = new CSVManager();
      int rc = csv->ReadFile(inpath);
      if(rc!=0){
	 delete csv;
	 return 1;
      }
     
      std::vector<std::string> data;
      csv->GetColumn_byIndex_str(0,data);
      prefix = data[0]; 
   
      int aRun=0; 
      const int N = data.size();
      for(int i=1;i<N;i++){
	 aRun = std::atoi( data[i].c_str() );
	 run.push_back(aRun); 
      }

      delete csv; 
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

      std::vector<std::string> dev,beam_state;
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
   void GetStatsWithCuts(std::vector<double> x,std::vector<double> y,
	 double cutLo,double cutHi,double &mean,double &stdev){
      // cuts are applied to the x variable. if true, compute stats on y 
      std::vector<double> Y;
      const int N = x.size();

      for(int i=0;i<N;i++){
	 if(x[i]>cutLo&&x[i]<cutHi) Y.push_back(y[i]);
      }

      mean  = math_df::GetMean<double>(Y);
      stdev = math_df::GetStandardDeviation<double>(Y);
   }
}
