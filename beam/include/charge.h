#ifndef UTIL_CHARGE_DF
#define UTIL_CHARGE_DF

// a struct to collect charge calculation results

#include <cstdlib>
#include <iostream> 

typedef struct charge { 
   double totalTime;   // total time of run (sec)  
   double value;       // charge in Coulombs 
   double error;       // uncertainty in Coulombs
   int run;            // run number 
   // constructor 
   charge(): 
      totalTime(0),value(0),error(0),run(0)
   {} 
   // useful functions 
   void Print(){
      std::cout << "-------------------------" << std::endl;
      std::cout << "run:        " << run       << std::endl; 
      std::cout << "Total time: " << Form("%.3lf sec",totalTime)     << std::endl;
      std::cout << "Charge:     " << Form("%.3E ± %.3E",value,error) << std::endl;
      std::cout << "-------------------------" << std::endl;
   }
} charge_t; 

#endif 
