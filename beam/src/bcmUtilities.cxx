#include "../include/bcmUtilities.h"
#include "CSVManager.cxx"
#include "Math.cxx"
#include "BCMManager.cxx"
namespace bcm_util {
   //______________________________________________________________________________
   TGraphErrors *GetTGraphErrors_byRunByUnserCurrent(std::string var,std::vector<scalerData_t> data){
      // get a plot of BCM scaler rate vs Unser current (on a run-by-run basis) 
      // note: this is a sub-optimal way to do a BCM calibration; there is no toggling of on/off 
      // of the beam current here. we utilize the unser calibration against the 
      // precision current source
      // you must provide the calibration numbers to the BCMManager before calling this function  
      std::vector<double> run,mean,stdev;
      // first get the BCM scaler rate vs run number 
      GetStats_byRun(var,data,run,mean,stdev); 
      // now get the unser current 
      run.clear(); // clear this since we're grabbing them again 
      std::vector<double> mean_ui,stdev_ui; 
      GetStats_byRun("unser.current",data,run,mean_ui,stdev_ui); 
      // now make the TGraphErrors plot 
      TGraphErrors *g = graph_df::GetTGraphErrors(mean_ui,stdev_ui,mean,stdev); 
      return g; 
   }
   //______________________________________________________________________________
   int GetStats_byRun(std::string var,std::vector<scalerData_t> data,
                      std::vector<double> &RUN,std::vector<double> &MEAN,std::vector<double> &STDEV){
      // get stats of var vs run number  
      int run_prev = data[0].runNumber;
      std::vector<double> v;
      double mean=0,stdev=0,theRun=0,theValue=0;
      const int NEV = data.size();
      for(int i=0;i<NEV;i++){
	 theRun   = data[i].runNumber;
         theValue = data[i].getValue(var);  
	 if(run_prev==theRun){
	    v.push_back(theValue);
	 }else{
	    // new run! compute stats 
	    mean  = math_df::GetMean<double>(v);
	    stdev = math_df::GetStandardDeviation<double>(v);
	    // save results
	    RUN.push_back(run_prev);
	    MEAN.push_back(mean);
	    STDEV.push_back(stdev);
	    // set up for next event 
	    v.clear();
	    // the current value counts for the "next" run that didn't match the previous 
	    v.push_back(theValue);
	 }
	 run_prev = theRun;
      }
      // compute stats on the last run  
      mean  = math_df::GetMean<double>(v);
      stdev = math_df::GetStandardDeviation<double>(v);
      // save results
      RUN.push_back(run_prev);
      MEAN.push_back(mean);
      STDEV.push_back(stdev);
      return 0;
   }
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
      std::cout << "time:       " << Form("%.3lf",data.time)  << std::endl;
      std::cout << "mean:       " << Form("%.3lf",data.mean)  << std::endl;
      std::cout << "stdev:      " << Form("%.3lf",data.stdev) << std::endl;
      std::cout << "----------------------------------------" << std::endl;
      return 0;
   }
   //______________________________________________________________________________
   int WriteToFile_cc(const char *outpath,std::vector<calibCoeff_t> data){
      // write the calibration coefficients to file
      const int NROW = data.size();
      const int NCOL = 5;
      std::vector<std::string> dev;
      std::vector<double> offset,offsetErr; 
      std::vector<double> gain,gainErr; 
      for(int i=0;i<NROW;i++){
	 dev.push_back( data[i].dev ); 
	 offset.push_back( data[i].offset ); 
	 offsetErr.push_back( data[i].offsetErr ); 
	 gain.push_back( data[i].slope ); 
	 gainErr.push_back( data[i].slopeErr ); 
      } 

      std::string header = "dev,offset,offsetErr,gain,gainErr";
 
      CSVManager *csv = new CSVManager(); 
      csv->InitTable(NROW,NCOL); 
      csv->SetColumn_str(0,dev); 
      csv->SetColumn<double>(1,offset);  
      csv->SetColumn<double>(2,offsetErr);  
      csv->SetColumn<double>(3,gain);  
      csv->SetColumn<double>(4,gainErr); 
      csv->SetHeader(header); 
      csv->WriteFile(outpath); 

      delete csv; 
 
      return 0;
   }
   //______________________________________________________________________________
   int WriteToFile(const char *outpath,std::vector<producedVariable_t> data){
      // write producedVariable data to file 
      std::vector<std::string> DEV,BEAM_STATE;
      std::vector<int> GRP;
      std::vector<double> MU,SIG,TIME;

      // write results to file
      const int N = data.size();
      for(int i=0;i<N;i++){
	 MU.push_back(data[i].mean);
	 SIG.push_back(data[i].stdev);
	 TIME.push_back(data[i].time); 
	 DEV.push_back(data[i].dev);
	 BEAM_STATE.push_back(data[i].beam_state);
	 GRP.push_back(data[i].group);
      }

      std::string header = "dev,beam_state,group,time,mean,stdev";

      const int NROW = DEV.size();
      const int NCOL = 6;
      CSVManager *csv = new CSVManager();
      csv->InitTable(NROW,NCOL);
      csv->SetColumn_str(0,DEV);
      csv->SetColumn_str(1,BEAM_STATE);
      csv->SetColumn<int>(2,GRP);
      csv->SetColumn<double>(3,TIME);
      csv->SetColumn<double>(4,MU);
      csv->SetColumn<double>(5,SIG);
      csv->SetHeader(header);
      csv->WriteFile(outpath);

      delete csv;
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
 
      std::vector<double> time,mean,stdev;  
      csv->GetColumn_byIndex<double>(3,time);
      csv->GetColumn_byIndex<double>(4,mean);
      csv->GetColumn_byIndex<double>(5,stdev);
     
      const int N = dev.size(); 
      producedVariable_t var; 
      for(int i=0;i<N;i++){
	 var.dev        = dev[i];
         var.beam_state = beam_state[i];  
         var.group      = group[i];  
         var.time       = time[i];  
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
   int LoadRuns(const char *inpath,TString &prefix,std::vector<codaRun_t> &runList){
      // load run list from a file 
      CSVManager *csv = new CSVManager();
      int rc = csv->ReadFile(inpath);
      if(rc!=0){
	 delete csv;
	 return 1;
      }
     
      std::vector<std::string> data,STREAM,SEG_BEG,SEG_END;
      csv->GetColumn_byIndex_str(0,data);
      csv->GetColumn_byIndex_str(1,STREAM);
      csv->GetColumn_byIndex_str(2,SEG_BEG);
      csv->GetColumn_byIndex_str(3,SEG_END);
      prefix = data[0];
  
      codaRun_t crun; 
      int aRun=0,aStr=0,aBeg=0,aEnd=0; 
      const int N = data.size();
      for(int i=1;i<N;i++){
	 aRun = std::atoi( data[i].c_str() );
	 aStr = std::atoi( STREAM[i].c_str() );
	 aBeg = std::atoi( SEG_BEG[i].c_str() );
	 aEnd = std::atoi( SEG_END[i].c_str() );
	 crun.runNumber    = aRun;
	 crun.stream       = aStr; 
	 crun.segmentBegin = aBeg; 
	 crun.segmentEnd   = aEnd; 
	 runList.push_back(crun);
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
   //______________________________________________________________________________
   TGraphErrors *GetTGraphErrors_byRun(std::string var,std::vector<scalerData_t> data){
      // make TGraphErrors of data as a function of run number 
      std::vector<double> run,mean,stdev;
      GetStats_byRun(var,data,run,mean,stdev); 
      TGraphErrors *g = graph_df::GetTGraphErrors(run,mean,stdev);
      return g;
   }
}
