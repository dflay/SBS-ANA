#ifndef UTIL_ROOTDATA_HH
#define UTIL_ROOTDATA_HH

// a useful data struct for containing 
// strings that describe a custom ROOT file 

#include <cstdlib>
#include <iostream> 

#include "TString.h"

namespace util {
   typedef struct rootData {
      TString fileName;          // name of file; includes full path!
      TString treeName;          // name of the ROOT tree 
      TString branchName;        // name of the ROOT branch 
      TString leafStructure;     // structure of the leaves of the branch
      // constructor 
      rootData():
	 fileName("NONE"),treeName("NONE"),branchName("NONE"),leafStructure("NONE")
      {}
      // useful functions 
      int Print(){
	 std::cout << Form("File name:      %s",fileName.Data())      << std::endl;
	 std::cout << Form("Tree name:      %s",treeName.Data())      << std::endl;
	 std::cout << Form("Branch name:    %s",branchName.Data())    << std::endl;
	 std::cout << Form("Leaf structure: %s",leafStructure.Data()) << std::endl;
      } 
   } rootData_t;
} //::util

#endif
