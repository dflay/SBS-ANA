#include "../include/bcmUtilities.h"
#include "CSVManager.cxx"
#include "Math.cxx"
//______________________________________________________________________________
namespace bcm_util {
   //______________________________________________________________________________
   int GetData(std::string var,std::vector<producedVariable_t> data,std::vector<double> &x,std::vector<double> &dx){
      // pull out the mean and stdev from the produced variable set
      const int N = data.size();
      for(int i=0;i<N;i++){
	 if(var.compare(data[i].dev)==0){
	    x.push_back(data[i].mean); 
	    dx.push_back(data[i].stdev); 
	 }
      }
      int NN = x.size();
      return NN; 
   }
   //______________________________________________________________________________
   int Print(producedVariable_t data){
      std::cout << "----------------------------------------" << std::endl;
      std::cout << "dev:        " << data.dev                 << std::endl;
      std::cout << "beam_state: " << data.beam_state          << std::endl;
      std::cout << "group:      " << data.group               << std::endl;
      std::cout << "mean:       " << Form("%.3lf",data.mean)  << std::endl;
      std::cout << "stdev:      " << Form("%.3lf",data.stdev) << std::endl;
      std::cout << "----------------------------------------" << std::endl;
      return 0;
   }
   //______________________________________________________________________________
   int LoadProducedVariables(const char *inpath,std::vector<producedVariable_t> &data){
      // load variables that have been cut on => produced variable  
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
      // load configuration paths  
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
   //______________________________________________________________________________
   int SubtractBaseline(std::vector<producedVariable_t> on,std::vector<producedVariable_t> off, 
                        std::vector<producedVariable_t> &diff,bool isDebug){
      // compute the quantity (beam-on) - (beam-off) for all variables
      // only compute this for producedVariables with matching group numbers and names  
      int cntr=0;
      double arg=0,argErr=0;
      producedVariable_t x;
      const int NOFF = off.size();
      const int NON  = on.size();
      for(int i=0;i<NON;i++){
	 for(int j=0;j<NOFF;j++){
	    if(on[i].group==off[j].group && on[i].dev.compare(off[j].dev)==0){
	       cntr++;
	       arg          = on[i].mean - off[j].mean;
	       argErr       = TMath::Sqrt( on[i].stdev*on[i].stdev + off[j].stdev*off[j].stdev );
	       x.mean       = arg;
	       x.stdev      = argErr;
	       x.dev        = on[i].dev;
	       x.beam_state = "DIFF";
	       x.group      = on[i].group;
	       diff.push_back(x); 
	       // print 
	       if(isDebug){
		  std::cout << Form("%s, group %d: on = %.3lf ± %.3lf, off = %.3lf ± %.3lf, on-off = %.3lf ± %.3lf",
			on[i].dev.c_str(),on[i].group,on[i].mean,on[i].stdev,off[i].mean,off[i].stdev,arg,argErr) << std::endl;
	       }
	    }
	 }
      }
      std::cout << "Found " << cntr << " BCM matches" << std::endl;
      return cntr;
   }
}
