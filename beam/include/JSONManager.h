#ifndef UTIL_DF_JSON_MANAGER_HH
#define UTIL_DF_JSON_MANAGER_HH

#include <cstdlib>
#include <iostream>
#include <fstream>

#include "json.hh"

using json = nlohmann::json;

namespace util_df {

   class JSONManager {

      private:
         json fObject;

      public:
         JSONManager(const char *filepath="NONE");
         ~JSONManager();

         int Print();
         int ReadFile(const char *filepath);

         bool DoesKeyExist(std::string keyName) const;

         int GetVectorFromKey_str(std::string key, int N, std::vector<std::string> &data);

         std::string GetValueFromKey_str(std::string key) const;
         std::string GetValueFromSubKey_str(std::string key,std::string subKey) const;

	 // templated methods 
	 template <typename T> T GetValueFromKey(std::string key) const {
	    bool exist = DoesKeyExist(key);
	    if(exist){
	       return (T)fObject[key];
	    }else{
	       return (T)(0);
	    }
	 }

         template <typename T> T GetValueFromSubKey(std::string key,std::string subKey) const {
            bool exist = DoesKeyExist(key);
            if(exist){
               return (T)fObject[key][subKey];
            }else{
               return (T)(0);
            }
         }

	 template <typename T>
	    int GetVectorFromKey(std::string key,int N,std::vector<T> &data){
	       // first check if the key exists
	       T arg;
	       bool exist = DoesKeyExist(key);
	       if(exist){
		  // found the key, fill the vector
		  for(int i=0;i<N;i++){
		     arg = (T)fObject[key][i];
		     data.push_back(arg);
		  }
	       }else{
		  return 1;
	       }
	       return 0;
	    }

   };

} // ::util_df


#endif

