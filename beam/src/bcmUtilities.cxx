#include "../include/bcmUtilities.h"
#include "CSVManager.cxx"
//______________________________________________________________________________
namespace bcm_util {
   //______________________________________________________________________________
   int LoadCuts(const char *inpath,std::vector<cut_t> &data){
      // load cuts from a file into a vector of type cut_t  
      CSVManager *csv = new CSVManager();
      int rc = csv->ReadFile("./input/cut-list.csv",true);
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
}
