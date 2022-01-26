#ifndef UTIL_ROOTFILE_MANAGER_H
#define UTIL_ROOTFILE_MANAGER_H

// a ROOT file manager

#include <cstdlib>
#include <iostream>
#include <string> 

#include "TChain.h"
#include "TTree.h"
#include "TFile.h"

#include "rootData.h"

// #include "CSVManager.h"

namespace util {
   class ROOTFileManager { 

      private:
	 // rootData_t fMetaData;
	 bool fIsDebug;
	 std::vector<CSVManager *> fData;     // main data container 

         std::vector<std::string> fTreeName;
         std::vector<std::vector<std::string>> fBranchName; 
         std::vector<std::vector<char>> fBufSize; 

         int CheckFile(const char *filePath); 
         int LoadDataFromTree(const char *filePath,const char *treeName,std::vector<std::string> branch,std::vector<char> bufSize);  

      public: 
	 ROOTFileManager();
	 ~ROOTFileManager();

         // void SetMetaData(rootData_t data) { fMetaData = data; }

         void SetDebug(bool v=true) { fIsDebug = v; } 

	 int LoadFile(rootData_t data);
	 int LoadFileStructure(const char *inpath);
	 int Clear(); 

         int Print(){
	    int N = fData.size();
            for(int i=0;i<N;i++){
	       std::cout << Form("Tree: %s",fTreeName[i].c_str()) << std::endl;
	       fData[i]->Print();
            }
	    return 0;
         }
 
	 // templated methods
	 template<typename T> 
	    int GetVector(const char *treeName,const char *branchName,std::vector<T> &out){
	       // find the index corresponding to the tree name
               std::string name = treeName;
	       int ii=0; 
               int NT = fTreeName.size(); 
               for(int i=0;i<NT;i++){
		  if(name.compare(fTreeName[i])==0) ii = i;
               }
	       // get the branch data 
	       fData[ii]->GetColumn_byName<T>(branchName,out); 
	       return 0;
	    } 

   };  

} //::util 

#endif 
