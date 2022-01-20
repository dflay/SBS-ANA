#ifndef DF_UTIL_H
#define DF_UTIL_H

#include <cstdlib>
#include <iostream>
#include <string>
#include <dirent.h>
#include <sys/types.h>

namespace util_df {
   void ListDirectory(const char *path,std::vector<std::string> &list);
   int SplitString(const char delim,const std::string myStr,std::vector<std::string> &out); 
   int GetROOTFileMetaData(const char *rfDirPath,int run,std::vector<int> &data,bool isDebug=false); 
} 

#endif 
