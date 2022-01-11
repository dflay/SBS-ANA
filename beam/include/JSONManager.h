#ifndef UTIL_DF_JSON_MANAGER_HH
#define UTIL_DF_JSON_MANAGER_HH

// A wrapper type class to streamline handling of JSON file data 
// based on the include file: 
// /u/site/12gev_phys/2.5/Linux_CentOS7.7.1908-gcc9.2.0/root/6.24.06/include/nlohmann/json.hpp
// since this is in ROOT already, don't need to explicitly include it here

#include <cstdlib>
#include <iostream>
#include <fstream>

using json = nlohmann::json;

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

#endif

