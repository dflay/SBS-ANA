#ifndef UTIL_ROOTFILE_MANAGER_H
#define UTIL_ROOTFILE_MANAGER_H

// a ROOT file manager

#include <cstdlib>
#include <iostream>
#include <string> 

#include "TFile.h"

#include "rootData.h"

// #include "CSVManager.h"

namespace util {
   class ROOTFileManager { 

      private:
	 rootData_t fMetaData;  // structure of the ROOT file  
	 CSVManager *fData;     // main data container 

         int CheckFile(const char *filePath); 
         int LoadDataFromTree(const char *filePath,const char *treeName,std::vector<std::string> leafStructure);  

      public: 
	 ROOTFileManager();
	 ~ROOTFileManager();

         void SetMetaData(rootData_t data) { fMetaData = data; }

	 int LoadFile(rootData_t data);

	 // templated methods
	 template<typename T> 
	    int GetVector(const char *branchName,std::vector<T> &out){
	       fData->GetColumn_byName<T>(branchName,out); 
	       return 0;
	    } 

   };  

} //::util 

#endif 
