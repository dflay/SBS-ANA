#include "../include/Utilities.h"
//______________________________________________________________________________
namespace util_df {
   //______________________________________________________________________________
   void ListDirectory(const char *path,std::vector<std::string> &list){
      struct dirent *entry;
      DIR *dir = opendir(path);

      if(dir==NULL){
	 return;
      }

      std::string myStr;
      while ((entry = readdir(dir)) != NULL) {
	 myStr = entry->d_name;
	 list.push_back(myStr);
      }
      closedir(dir);
   }
   //______________________________________________________________________________
   int SplitString(const char delim,const std::string myStr,std::vector<std::string> &out){
      // split a string by a delimiter
      std::stringstream ss(myStr);
      std::vector<std::string> result;

      while( ss.good() ){
	 std::string substr;
	 std::getline(ss,substr,delim);
	 out.push_back(substr);
      }
      return 0;
   }
   //______________________________________________________________________________
   int GetROOTFileMetaData(const char *rfDirPath,int run,std::vector<int> &data,bool isDebug){
      // determine the beginning and end segment number for a CODA run
      // input: 
      // - rfDirPath: path to where the ROOT file(s) are located 
      // - run: CODA run number 
      // output: 
      // - begSeg: start segment number of the ROOT file associated with run 
      // - endSeg: ending segment number of the ROOT file associated with run

      int rc=-1; // assume fail 

      // first get list of files in the directory 
      std::vector<std::string> fileList; 
      ListDirectory(rfDirPath,fileList); 
      
      // skip first two entries since we get . and .. at top of list
      for(int i=0;i<2;i++) fileList.erase(fileList.begin()); 
 
      // identify the run of interest
      int theRun=0,stream=-1,begSeg=-1,endSeg=-1;
      std::string theStr; 
      std::vector<std::string> o1,o2; 
      const int NF = fileList.size();
      for(int i=0;i<NF;i++){  
	 // split each entry based on a sufficient delimiter 
	 SplitString('_',fileList[i],o1);
	 // 3rd entry (index 2) is the one that has the run number 
	 theRun = std::atoi(o1[2].c_str()); 
         if(theRun==run){
	    // found the  run
	    rc = 0;
            // determine the stream (index 3) 
            theStr = o1[3]; 
            SplitString('m',theStr,o2); 
            stream = std::atoi(o2[1].c_str());
	    o2.clear();
	    // determine the segment numbers (index 4 and 5) 
	    theStr = o1[4]; 
            SplitString('g',theStr,o2); 
            begSeg = std::atoi(o2[1].c_str());  
            endSeg = std::atoi(o1[5].c_str());
	    if(isDebug){
	       std::cout << Form("[Utilities::GetROOTFileMetaData]: Found run %d, stream = %d, begSeg = %d, endSeg = %d", 
		                 theRun,stream,begSeg,endSeg) << std::endl;
	    }
	    // store results
	    data.push_back(stream);
	    data.push_back(begSeg);
	    data.push_back(endSeg);  
	 }
	 // set up for next file
	 o1.clear();
	 o2.clear(); 
      } 
      return rc;
   }
} // ::util
