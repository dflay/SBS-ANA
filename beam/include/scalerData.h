#ifndef UTIL_SCALER_DATA_H
#define UTIL_SCALER_DATA_H

// a data struct for scaler data 

typedef struct scalerData {
   std::string arm;       // arm (Left, sbs, or E) 
   std::string info;      // user notes  
   double time;           // time in seconds                    
   double time_num;       // time numerator: clock counts                
   double time_den;       // time denominator: clock rate
   double unserRate;      // Unser rate                  
   double unserCounts;    // Unser counts                
   double unserCurrent;   // Unser current                
   double u1Rate;         // upstream BCM x1 (analog) rate           
   double u1Counts;       // upstream BCM x1 (analog) counts           
   double u1Current;      // upstream BCM x1 (analog) current           
   double unewRate;       // upstream BCM x1 (digital) rate               
   double unewCounts;     // upstream BCM x1 (digital) counts                
   double unewCurrent;    // upstream BCM x1 (digital) current                
   double dnewRate;       // downstream BCM x1 (digital) rate               
   double dnewCounts;     // downstream BCM x1 (digital) counts                
   double dnewCurrent;    // downstream BCM x1 (digital) current               
   double d1Rate;         // downstream BCM x1 (analog) rate                 
   double d1Counts;       // downstream BCM x1 (analog) counts               
   double d1Current;      // downstream BCM x1 (analog) current               
   double d3Rate;         // downstream BCM x3 (analog) rate             
   double d3Counts;       // downstream BCM x3 (analog) counts            
   double d3Current;      // downstream BCM x3 (analog) current        
   double d10Rate;        // downstream BCM x10 (analog) rate            
   double d10Counts;      // downstream BCM x10 (analog) counts       
   double d10Current;     // downstream BCM x10 (analog) current        
   int runNumber;
   int event;
   // constructor 
   scalerData():
      arm("NONE"),info("NONE"),time(0),time_num(0),time_den(0),
      unserRate(0),unserCounts(0),unserCurrent(0),
      u1Rate(0),u1Counts(0),u1Current(0),unewRate(0),unewCounts(0),unewCurrent(0),
      dnewRate(0),dnewCounts(0),dnewCurrent(0),d1Rate(0),d1Counts(0),d1Current(0),
      d3Rate(0),d3Counts(0),d3Current(0),d10Rate(0),d10Counts(0),d10Current(0),
      runNumber(0),event(0)
   {} 
   // get value of the member variable based on name 
   double getValue(std::string varName){
      double val = 0;
      if(varName.compare("run")==0)            val = runNumber;
      if(varName.compare("event")==0)          val = event;
      if(varName.compare("time")==0)           val = time;
      if(varName.compare("unser.rate")==0)     val = unserRate;
      if(varName.compare("unser.cnt")==0)      val = unserCounts;
      if(varName.compare("unser.current")==0)  val = unserCurrent;
      if(varName.compare("u1.rate")==0)        val = u1Rate;
      if(varName.compare("u1.cnt")==0)         val = u1Counts;
      if(varName.compare("u1.current")==0)     val = u1Current;
      if(varName.compare("unew.rate")==0)      val = unewRate;
      if(varName.compare("unew.cnt")==0)       val = unewCounts;
      if(varName.compare("unew.current")==0)   val = unewCurrent;
      if(varName.compare("d1.rate")==0)        val = d1Rate;
      if(varName.compare("d1.cnt")==0)         val = d1Counts;
      if(varName.compare("d1.current")==0)     val = d1Current;
      if(varName.compare("d3.rate")==0)        val = d3Rate;
      if(varName.compare("d3.cnt")==0)         val = d3Counts;
      if(varName.compare("d3.current")==0)     val = d3Current;
      if(varName.compare("d10.rate")==0)       val = d10Rate;
      if(varName.compare("d10.cnt")==0)        val = d10Counts;
      if(varName.compare("d10.current")==0)    val = d10Current;
      if(varName.compare("dnew.rate")==0)      val = dnewRate;
      if(varName.compare("dnew.cnt")==0)       val = dnewCounts;
      if(varName.compare("dnew.current")==0)   val = dnewCurrent;
      return val;
   }
   // print data to screen 
   int Print(std::string type){
      if(type.compare("rate")==0){
	 std::cout << Form("event %03d, "       ,event)
	           << Form("run %05d, "         ,runNumber)
	           << Form("time = %.3lf, "     ,time)
	           << Form("time_num = %.3lf, " ,time_num)
	           << Form("time_den = %.3lf, " ,time_den)
	           << Form("unser rate = %.3lf ",unserRate)
	           << Form("u1 rate = %.3lf "   ,u1Rate)
	           << Form("unew rate = %.3lf " ,unewRate)
	           << Form("d1 rate = %.3lf "   ,d1Rate)
	           << Form("d3 rate = %.3lf "   ,d3Rate)
	           << Form("d10 rate = %.3lf "  ,d10Rate)
	           << Form("dnew rate = %.3lf " ,dnewRate) << std::endl;
      }else if(type.compare("cnt")==0 || type.compare("count")==0){
	 std::cout << Form("event %03d, "      ,event)
	           << Form("run %05d, "        ,runNumber)
	           << Form("time = %.3lf, "    ,time)
	           << Form("time_num = %.3lf, ",time_num)
	           << Form("time_den = %.3lf, ",time_den)
	           << Form("unser cnt = %.3lf ",unserCounts)
	           << Form("u1 cnt = %.3lf "   ,u1Counts)
	           << Form("unew cnt = %.3lf " ,unewCounts)
	           << Form("d1 cnt = %.3lf "   ,d1Counts)
	           << Form("d3 cnt = %.3lf "   ,d3Counts)
	           << Form("d10 cnt = %.3lf "  ,d10Counts)
	           << Form("dnew cnt = %.3lf " ,dnewCounts) << std::endl;
      }else if(type.compare("current")==0){
	 std::cout << Form("event %03d, "          ,event)
	           << Form("run %05d, "            ,runNumber)
	           << Form("time = %.3lf, "        ,time)
	           << Form("time_num = %.3lf, "    ,time_num)
	           << Form("time_den = %.3lf, "    ,time_den)
	           << Form("unser current = %.3lf ",unserCurrent)
	           << Form("u1 current = %.3lf "   ,u1Current)
	           << Form("unew current = %.3lf " ,unewCurrent)
	           << Form("d1 current = %.3lf "   ,d1Current)
	           << Form("d3 current = %.3lf "   ,d3Current)
	           << Form("d10 current = %.3lf "  ,d10Current)
	           << Form("dnew current = %.3lf " ,dnewCurrent) << std::endl;
      }
      return 0;
   } 
} scalerData_t; 

#endif 
