#include "../include/BCMManager.h"
//______________________________________________________________________________
BCMManager::BCMManager(const char *filePath,bool isDebug,const char *ccFilePath){
   fIsDebug          = isDebug; 
   fCalculateCurrent = false;  
   fEvtCntrLeft      = 0;  
   fEvtCntrSBS       = 0;  
   fEvtCntrEPICS     = 0; 
   fLastTimeLeft     = 0; 
   fLastTimeSBS      = 0;

   std::string path = filePath; 
   if(path.compare("NONE")!=0){
      LoadFile(filePath);
   }

   std::string cc_filePath = ccFilePath; 
   if(cc_filePath.compare("NONE")!=0){
      fCalculateCurrent = true;
      LoadCalibrationCoefficients(ccFilePath);
   } 
 
}
//______________________________________________________________________________
BCMManager::~BCMManager(){
   Clear();
}
//______________________________________________________________________________
void BCMManager::Clear(){
   fLeft.clear();
   fSBS.clear();
   fEPICS.clear(); 
   fEvtCntrLeft  = 0;
   fEvtCntrSBS   = 0;
   fEvtCntrEPICS = 0;
   fLastTimeLeft = 0; 
   fLastTimeSBS  = 0; 
}
//______________________________________________________________________________
void BCMManager::LoadFile(const char *filePath,int runNumber){
   // Attach trees for LHRS and SBS 
   LoadDataFromTree(filePath,"TSLeft",runNumber); 
   LoadDataFromTree(filePath,"TSsbs" ,runNumber);
   LoadEPICSDataFromTree(filePath,runNumber);  
}
//______________________________________________________________________________
void BCMManager::LoadDataFromTree(const char *filePath,const char *treeName,int runNumber){

   if(fIsDebug) std::cout << "[BCMManager::LoadDataFromTree]: Loading data from tree: " << treeName << std::endl;

   std::string arm; 
   std::string name = treeName;
   if(name.compare("TSLeft")==0) arm = "Left"; 
   if(name.compare("TSsbs")==0)  arm = "sbs"; 
 
   double time=0,time_num=0,time_den=0; 
   double unser_rate=0,unser_cnt=0;
   double u1_rate=0,u1_cnt=0;
   double unew_rate=0,unew_cnt=0;
   double dnew_rate=0,dnew_cnt=0;
   double d1_rate=0,d1_cnt=0;
   double d3_rate=0,d3_cnt=0;
   double d10_rate=0,d10_cnt=0;

   TChain *ch = new TChain(treeName); 
   ch->Add(filePath);
   int NN = ch->GetEntries(); 
   TTree *aTree = ch->GetTree();
   aTree->SetBranchAddress(Form("%s.bcm.unser.cnt" ,arm.c_str()),&unser_cnt );
   aTree->SetBranchAddress(Form("%s.bcm.unser.rate",arm.c_str()),&unser_rate);
   aTree->SetBranchAddress(Form("%s.bcm.u1.cnt"    ,arm.c_str()),&u1_cnt    );
   aTree->SetBranchAddress(Form("%s.bcm.u1.rate"   ,arm.c_str()),&u1_rate   );
   aTree->SetBranchAddress(Form("%s.bcm.unew.cnt"  ,arm.c_str()),&unew_cnt  );
   aTree->SetBranchAddress(Form("%s.bcm.unew.rate" ,arm.c_str()),&unew_rate );
   aTree->SetBranchAddress(Form("%s.bcm.d1.cnt"    ,arm.c_str()),&d1_cnt    );
   aTree->SetBranchAddress(Form("%s.bcm.d1.rate"   ,arm.c_str()),&d1_rate   );
   aTree->SetBranchAddress(Form("%s.bcm.d3.cnt"    ,arm.c_str()),&d3_cnt    );
   aTree->SetBranchAddress(Form("%s.bcm.d3.rate"   ,arm.c_str()),&d3_rate   );
   aTree->SetBranchAddress(Form("%s.bcm.d10.cnt"   ,arm.c_str()),&d10_cnt   );
   aTree->SetBranchAddress(Form("%s.bcm.d10.rate"  ,arm.c_str()),&d10_rate  );
   aTree->SetBranchAddress(Form("%s.bcm.dnew.cnt"  ,arm.c_str()),&dnew_cnt  );
   aTree->SetBranchAddress(Form("%s.bcm.dnew.rate" ,arm.c_str()),&dnew_rate );
   if(arm.compare("Left")==0){
      aTree->SetBranchAddress(Form("Left.104kHz_CLK.cnt")       ,&time_num);
      aTree->SetBranchAddress(Form("Left.104kHz_CLK.rate")      ,&time_den);
   }else if(arm.compare("sbs")==0){
      aTree->SetBranchAddress(Form("sbs.BBCalHi.RF.scaler")     ,&time_num);
      aTree->SetBranchAddress(Form("sbs.BBCalHi.RF.scalerRate") ,&time_den);
   }

   scalerData_t pt; 

   for(int i=0;i<NN;i++){
      aTree->GetEntry(i);
      if(time_den!=0){
	 time = time_num/time_den; 
      }else{
	 time = 0;  
      } 
      pt.time_num    = time_num;  
      pt.time_den    = time_den;  
      pt.arm         = arm; 
      pt.runNumber   = runNumber; 
      pt.unserRate   = unser_rate;  
      pt.unserCounts = unser_cnt;  
      pt.u1Rate      = u1_rate;  
      pt.u1Counts    = u1_cnt;  
      pt.unewRate    = unew_rate;  
      pt.unewCounts  = unew_cnt;  
      pt.dnewRate    = dnew_rate;  
      pt.dnewCounts  = dnew_cnt;  
      pt.d1Rate      = d1_rate;  
      pt.d1Counts    = d1_cnt;  
      pt.d3Rate      = d3_rate;  
      pt.d3Counts    = d3_cnt;  
      pt.d10Rate     = d10_rate;  
      pt.d10Counts   = d10_cnt; 
      if(fCalculateCurrent){
	 ApplyCalibrationCoeff(pt);
      }
      if(arm.compare("Left")==0){
	 pt.time  = fLastTimeLeft + time; 
	 pt.event = fEvtCntrLeft; 
	 fLeft.push_back(pt); 
	 fEvtCntrLeft++;
      }else if(arm.compare("sbs")==0){
	 pt.time  = fLastTimeSBS + time; 
	 pt.event = fEvtCntrSBS; 
	 fSBS.push_back(pt); 
	 fEvtCntrSBS++;
      }
   }
  
   // track the last time registered 
   double lastTime=0; 
   if(arm.compare("Left")==0){
      if( IsBad(fLeft[NN-1].time) ){
         fLeft[NN-1].time = 0; 
	 std::cout << "[BCMManager::LoadDataFromTree]: WARNING! arm = " << arm << ", bad last time! Setting to zero." << std::endl;
      }
      fLastTimeLeft = fLeft[NN-1].time;
      lastTime = fLastTimeLeft; 
   }else if(arm.compare("SBS")==0){
      if( IsBad(fSBS[NN-1].time) ){
	 std::cout << "[BCMManager::LoadDataFromTree]: WARNING! arm = " << arm << ", bad last time! Setting to zero." << std::endl;
         fSBS[NN-1].time = 0; 
      } 
      fLastTimeSBS  = fSBS[NN-1].time;
      lastTime      = fLastTimeSBS; 
   }

   if(fIsDebug) std::cout << "The last time is: " << lastTime << std::endl;
 
   delete aTree; 
   delete ch; 
}
//______________________________________________________________________________
int BCMManager::LoadCalibrationCoefficients(const char *filePath){
   CSVManager *csv = new CSVManager();
   csv->ReadFile(filePath,true); 

   std::vector<std::string> dev; 
   std::vector<double> ped,pedErr,offset,offsetErr,gain,gainErr;

   csv->GetColumn_byName_str("dev",dev); 
   csv->GetColumn_byName<double>("pedestal_Hz"   ,ped); 
   csv->GetColumn_byName<double>("pedestalErr_Hz",pedErr); 
   csv->GetColumn_byName<double>("offset_Hz"     ,offset); 
   csv->GetColumn_byName<double>("offsetErr_Hz"  ,offsetErr); 
   csv->GetColumn_byName<double>("gain_HzuA"     ,gain); 
   csv->GetColumn_byName<double>("gainErr_HzuA"  ,gainErr); 

   calibCoeff_t cc; 
   const int ND = dev.size();
   for(int i=0;i<ND;i++){
      cc.dev         = dev[i];
      cc.pedestal    = ped[i];  
      cc.pedestalErr = pedErr[i];  
      cc.offset      = offset[i];  
      cc.offsetErr   = offsetErr[i];  
      cc.slope       = gain[i];  
      cc.slopeErr    = gainErr[i];  
      fCC.push_back(cc); 
   }
   delete csv;
   return 0; 
}
//______________________________________________________________________________
int BCMManager::ApplyCalibrationCoeff(scalerData_t &data){
   // apply calibration coefficients to all data
   std::vector<std::string> dev;
   dev.push_back("unser"); 
   dev.push_back("u1"); 
   dev.push_back("unew"); 
   dev.push_back("d1"); 
   dev.push_back("d3"); 
   dev.push_back("d10"); 
   dev.push_back("dnew");

   double PED=0,OFFSET=0,GAIN=0;
   std::vector<double> v,dv;  

   int rc=0; // if return code = 0, we found the calibration coefficients, apply them 
 
   const int ND = dev.size();
   for(int i=0;i<ND;i++){
      rc = GetCalibrationCoeff(dev[i],v,dv);
      PED = v[0]; OFFSET = v[1]; GAIN = v[2];
      if(rc==0){
	 if(dev[i].compare("unser")==0){
	    data.unserCurrent = (data.unserRate - PED - OFFSET)/GAIN;
	 }else if(dev[i].compare("u1")==0){
	    data.u1Current = (data.u1Rate - PED - OFFSET)/GAIN;
	 }else if(dev[i].compare("unew")==0){
	    data.unewCurrent = (data.unewRate - PED - OFFSET)/GAIN;
	 }else if(dev[i].compare("d1")==0){
	    data.d1Current = (data.d1Rate - PED - OFFSET)/GAIN;
	 }else if(dev[i].compare("d3")==0){
	    data.d3Current = (data.d3Rate - PED - OFFSET)/GAIN;
	 }else if(dev[i].compare("d10")==0){
	    data.d10Current = (data.d10Rate - PED - OFFSET)/GAIN;
	 }else if(dev[i].compare("dnew")==0){
	    data.dnewCurrent = (data.dnewRate - PED - OFFSET)/GAIN;
	 }
      }
      // set up for next device 
      v.clear(); dv.clear(); 
   } 
   return 0;
}
//______________________________________________________________________________
int BCMManager::GetCalibrationCoeff(std::string dev,std::vector<double> &v,std::vector<double> &dv){
   // get calibration coefficients by device
   int rc=0;  
   const int N = fCC.size();
   for(int i=0;i<N;i++){
      if(dev.compare(fCC[i].dev)==0){
	 v.push_back(fCC[i].pedestal);
	 v.push_back(fCC[i].offset);  
	 v.push_back(fCC[i].slope);  
	 dv.push_back(fCC[i].pedestalErr);
	 dv.push_back(fCC[i].offsetErr);  
	 dv.push_back(fCC[i].slopeErr);  
      }
   }
   // check if we found data
   int NV = v.size();
   if(NV==0){
      std::cout << "[BCMManager::GetCalibrationCoeff]: WARNING! No calibration coefficients for device = " << dev << "! "; 
      std::cout << "Defaulting to ped = 0, offset = 0, gain = 1" << std::endl;
      v.push_back(0);
      v.push_back(0);
      v.push_back(1);
      dv.push_back(0);
      dv.push_back(0);
      dv.push_back(0);
      rc = 1;
   }
   // check gain value (avoid divide by zero error) 
   if(v[2]==0){
      std::cout << "[BCMManager::GetCalibrationCoeff]: WARNING! Gain = 0 for device = " << dev << "! ";
      std::cout << "Setting to 1." << std::endl;
      v[2] = 1; 
      dv[2] = 0; 
      rc = 1;
   }
   return rc;
}
//______________________________________________________________________________
bool BCMManager::IsBad(double v){
   return std::isinf(v) || std::isnan(v); 
}
//______________________________________________________________________________
void BCMManager::LoadEPICSDataFromTree(const char *filePath,int runNumber){

   if(fIsDebug) std::cout << "[BCMManager::LoadEPICSDataFromTree]: Loading data from tree: E" << std::endl; 

   double epicsTime=0,IBC1H04CRCUR2=0,hac_bcm_average=0;
   double IPM1H04A_XPOS=0,IPM1H04A_YPOS=0; 
   double IPM1H04E_XPOS=0,IPM1H04E_YPOS=0; 

   TChain *ch = new TChain("E"); 
   ch->Add(filePath);
   int NL = ch->GetEntries(); 
   TTree *aTree = ch->GetTree();
   aTree->SetBranchAddress("hac_bcm_average",&hac_bcm_average);
   aTree->SetBranchAddress("IPM1H04A.XPOS"  ,&IPM1H04A_XPOS  );
   aTree->SetBranchAddress("IPM1H04A.YPOS"  ,&IPM1H04A_YPOS  );
   aTree->SetBranchAddress("IPM1H04E.XPOS"  ,&IPM1H04E_XPOS  );
   aTree->SetBranchAddress("IPM1H04E.YPOS"  ,&IPM1H04E_YPOS  );
   aTree->SetBranchAddress("IBC1H04CRCUR2"  ,&IBC1H04CRCUR2  );
   aTree->SetBranchAddress("timestamp"      ,&epicsTime      );

   epicsData_t pt; 
   for(int i=0;i<NL;i++){
      aTree->GetEntry(i); 
      pt.event           = fEvtCntrEPICS; 
      pt.runNumber       = runNumber;
      pt.time            = epicsTime;
      pt.hac_bcm_average = hac_bcm_average; 
      pt.IBC1H04CRCUR2   = IBC1H04CRCUR2; 
      pt.IPM1H04A_XPOS   = IPM1H04A_XPOS;   
      pt.IPM1H04A_YPOS   = IPM1H04A_YPOS;   
      pt.IPM1H04E_XPOS   = IPM1H04E_XPOS;   
      pt.IPM1H04E_YPOS   = IPM1H04E_YPOS; 
      fEPICS.push_back(pt);  
      fEvtCntrEPICS++; 
   }

   delete aTree;
   delete ch; 

}
//______________________________________________________________________________
TH1F * BCMManager::GetTH1F(const char *arm,const char *var_name,int NBin,double min,double max){
   std::vector<double> x;
   GetVector(arm,var_name,x); 

   TString name = Form("%s.%s",arm,var_name);
   TH1F *h = new TH1F(name,name,NBin,min,max); 
   const int N = x.size();
   for(int i=0;i<N;i++) h->Fill(x[i]); 

   return h; 
}
//______________________________________________________________________________
TH2F * BCMManager::GetTH2F(const char *arm,const char *xAxis,const char *yAxis,
                           int NXBin,double xMin,double xMax,int NYBin,double yMin,double yMax){

   std::vector<double> x,y;
   GetVector(arm,xAxis,x); 
   GetVector(arm,yAxis,y); 

   TString name = Form("%s.%s:%s.%s",arm,yAxis,arm,xAxis);
   TH2F *h = new TH2F(name,name,NXBin,xMin,xMax,NYBin,yMin,yMax); 
   const int N = x.size();
   for(int i=0;i<N;i++) h->Fill(x[i],y[i]); 

   return h;
}
//______________________________________________________________________________
TGraph * BCMManager::GetTGraph(const char *arm,const char *xAxis,const char *yAxis){
   // get a TGraph object 
   std::string ARM = arm; 
   TString xAxisName = Form("%s",xAxis);
   TString yAxisName = Form("%s",yAxis); 

   if(fIsDebug) std::cout << xAxisName << " " << yAxisName << std::endl;

   std::vector<double> x,y; 
   GetVector(arm,xAxisName.Data(),x); 
   GetVector(arm,yAxisName.Data(),y);

   const int N = x.size(); 
   TGraph *g = new TGraph(N,&x[0],&y[0]); 
   return g; 
}
//______________________________________________________________________________
int BCMManager::GetVector_scaler(const char *arm,std::vector<scalerData_t> &data){
   // return a vector of the scalerData 
   std::string armName = arm; 
 
   int NN=0;
   if(armName.compare("Left")==0) NN = fLeft.size(); 
   if(armName.compare("sbs")==0)  NN = fSBS.size(); 

   for(int i=0;i<NN;i++){
      if(armName.compare("Left")==0){
	 data.push_back(fLeft[i]); 
      }else if(armName.compare("sbs")==0){
	 data.push_back(fSBS[i]); 
      }
   } 
   return 0;
}
//______________________________________________________________________________
int BCMManager::GetVector_epics(std::vector<epicsData_t> &data){
   // return a vector of the epicsData 
   int NN = fEPICS.size();
   for(int i=0;i<NN;i++){
      data.push_back(fEPICS[i]); 
   } 
   return 0;
}
//______________________________________________________________________________
int BCMManager::GetVector(const char *arm,const char *var,std::vector<double> &v){
   // fill a vector with the variable 

   std::string armName = arm;
   std::string varName = var;

   // if(fIsDebug) std::cout << "arm = " << arm << ", var = " << var << std::endl;

   int NN=0;
   if(armName.compare("Left")==0) NN = fLeft.size(); 
   if(armName.compare("sbs")==0)  NN = fSBS.size(); 
   if(armName.compare("E")==0)    NN = fEPICS.size(); 

   double val=0;
   for(int i=0;i<NN;i++){
      val = 0;
      if(armName.compare("Left")==0){
	 if(varName.compare("event")==0         ) val = fLeft[i].event; 
	 if(varName.compare("time")==0          ) val = fLeft[i].time; 
	 if(varName.compare("time_num")==0      ) val = fLeft[i].time_num; 
	 if(varName.compare("time_den")==0      ) val = fLeft[i].time_den; 
	 if(varName.compare("u1.cnt")==0        ) val = fLeft[i].u1Counts; 
	 if(varName.compare("unew.cnt")==0      ) val = fLeft[i].unewCounts; 
	 if(varName.compare("d1.cnt")==0        ) val = fLeft[i].d1Counts; 
	 if(varName.compare("d3.cnt")==0        ) val = fLeft[i].d3Counts; 
	 if(varName.compare("d10.cnt")==0       ) val = fLeft[i].d10Counts;
	 if(varName.compare("dnew.cnt")==0      ) val = fLeft[i].dnewCounts; 
	 if(varName.compare("unser.cnt")==0     ) val = fLeft[i].unserCounts; 
	 if(varName.compare("u1.rate")==0       ) val = fLeft[i].u1Rate;
	 if(varName.compare("unew.rate")==0     ) val = fLeft[i].unewRate;
	 if(varName.compare("d1.rate")==0       ) val = fLeft[i].d1Rate;
	 if(varName.compare("d3.rate")==0       ) val = fLeft[i].d3Rate;
	 if(varName.compare("d10.rate")==0      ) val = fLeft[i].d10Rate;
	 if(varName.compare("dnew.rate")==0     ) val = fLeft[i].dnewRate;
	 if(varName.compare("unser.rate")==0    ) val = fLeft[i].unserRate;
	 if(varName.compare("u1.current")==0    ) val = fLeft[i].u1Current;
	 if(varName.compare("unew.current")==0  ) val = fLeft[i].unewCurrent;
	 if(varName.compare("d1.current")==0    ) val = fLeft[i].d1Current;
	 if(varName.compare("d3.current")==0    ) val = fLeft[i].d3Current;
	 if(varName.compare("d10.current")==0   ) val = fLeft[i].d10Current;
	 if(varName.compare("dnew.current")==0  ) val = fLeft[i].dnewCurrent;
	 if(varName.compare("unser.current")==0 ) val = fLeft[i].unserCurrent;
      }else if(armName.compare("sbs")==0){
       	 if(varName.compare("event")==0         ) val = fSBS[i].event; 
	 if(varName.compare("time")==0          ) val = fSBS[i].time; 
	 if(varName.compare("time_num")==0      ) val = fSBS[i].time_num; 
	 if(varName.compare("time_den")==0      ) val = fSBS[i].time_den; 
	 if(varName.compare("u1.cnt")==0        ) val = fSBS[i].u1Counts; 
	 if(varName.compare("unew.cnt")==0      ) val = fSBS[i].unewCounts; 
	 if(varName.compare("d1.cnt")==0        ) val = fSBS[i].d1Counts; 
	 if(varName.compare("d3.cnt")==0        ) val = fSBS[i].d3Counts; 
	 if(varName.compare("d10.cnt")==0       ) val = fSBS[i].d10Counts;
	 if(varName.compare("dnew.cnt")==0      ) val = fSBS[i].dnewCounts; 
	 if(varName.compare("unser.cnt")==0     ) val = fSBS[i].unserCounts; 
	 if(varName.compare("u1.rate")==0       ) val = fSBS[i].u1Rate;
	 if(varName.compare("unew.rate")==0     ) val = fSBS[i].unewRate;
	 if(varName.compare("d1.rate")==0       ) val = fSBS[i].d1Rate;
	 if(varName.compare("d3.rate")==0       ) val = fSBS[i].d3Rate;
	 if(varName.compare("d10.rate")==0      ) val = fSBS[i].d10Rate;
	 if(varName.compare("dnew.rate")==0     ) val = fSBS[i].dnewRate;
	 if(varName.compare("unser.rate")==0    ) val = fSBS[i].unserRate;
	 if(varName.compare("u1.current")==0    ) val = fSBS[i].u1Current;
	 if(varName.compare("unew.current")==0  ) val = fSBS[i].unewCurrent;
	 if(varName.compare("d1.current")==0    ) val = fSBS[i].d1Current;
	 if(varName.compare("d3.current")==0    ) val = fSBS[i].d3Current;
	 if(varName.compare("d10.current")==0   ) val = fSBS[i].d10Current;
	 if(varName.compare("dnew.current")==0  ) val = fSBS[i].dnewCurrent;
	 if(varName.compare("unser.current")==0 ) val = fSBS[i].unserCurrent;
      }else if(armName.compare("E")==0){
         if(varName.compare("event")==0)           val = fEPICS[i].event; 
	 if(varName.compare("time")==0)            val = fEPICS[i].time; 
	 if(varName.compare("IPM1H04A.XPOS")==0)   val = fEPICS[i].IPM1H04A_XPOS; 
	 if(varName.compare("IPM1H04A.YPOS")==0)   val = fEPICS[i].IPM1H04A_YPOS; 
	 if(varName.compare("IPM1H04E.XPOS")==0)   val = fEPICS[i].IPM1H04E_XPOS; 
	 if(varName.compare("IPM1H04E.YPOS")==0)   val = fEPICS[i].IPM1H04E_YPOS; 
	 if(varName.compare("hac_bcm_average")==0) val = fEPICS[i].hac_bcm_average; 
	 if(varName.compare("IBC1H04CRCUR2")==0)   val = fEPICS[i].IBC1H04CRCUR2; 
      }
      v.push_back(val);
   }
  
   int NV = v.size();
   if(NV==0) std::cout << "[BCMManager::GetVector]: No events for arm = " << armName << ", variable = " << varName << "!" << std::endl;

   return 0;
}
//______________________________________________________________________________
void BCMManager::Print(const char *arm){

   std::string ARM = arm; 
   const int NL = fLeft.size();
   const int NS = fSBS.size();
   const int NE = fEPICS.size();
   int N=0;
   if(ARM.compare("Left")==0) N = NL; 
   if(ARM.compare("sbs")==0)  N = NS; 
   if(ARM.compare("E")==0)    N = NE; 

   for(int i=0;i<N;i++){
      if(ARM.compare("Left")==0){
	 PrintScaler(fLeft[i]); 
      }else if(ARM.compare("sbs")==0){
	 PrintScaler(fSBS[i]); 
      }else if(ARM.compare("E")==0){
	 PrintEPICS(fEPICS[i]); 
      } 
   }

}
//______________________________________________________________________________
void BCMManager::PrintScaler(scalerData_t data){
   std::cout << Form("event %03d, "       ,data.event) 
             << Form("run %05d, "         ,data.runNumber) 
             << Form("time = %.3lf, "     ,data.time) 
             << Form("time_num = %.3lf, " ,data.time_num) 
             << Form("time_den = %.3lf, " ,data.time_den) 
             << Form("unser rate = %.3lf ",data.unserRate) 
             << Form("u1 rate = %.3lf "   ,data.u1Rate) 
             << Form("unew rate = %.3lf " ,data.unewRate) 
             << Form("dnew rate = %.3lf " ,data.dnewRate) 
             << Form("d1 rate = %.3lf "   ,data.d1Rate) 
             << Form("d3 rate = %.3lf "   ,data.d3Rate) 
             << Form("d10 rate = %.3lf "  ,data.d10Rate) << std::endl;
}
//______________________________________________________________________________
void BCMManager::PrintEPICS(epicsData_t data){
   std::cout << Form("event %03d, "             ,data.event) 
             << Form("run %05d, "               ,data.runNumber) 
             << Form("time = %.3lf, "           ,data.time) 
             << Form("IPM1H04A_XPOS = %.3lf, "  ,data.IPM1H04A_XPOS) 
             << Form("IPM1H04A_YPOS = %.3lf, "  ,data.IPM1H04A_YPOS) 
             << Form("IPM1H04E_XPOS = %.3lf, "  ,data.IPM1H04E_XPOS) 
             << Form("IPM1H04E_YPOS = %.3lf, "  ,data.IPM1H04E_YPOS) 
             << Form("hac_bcm_average = %.3lf, ",data.hac_bcm_average) 
             << Form("IPM1H04CRCUR2 = %.3lf, "  ,data.IBC1H04CRCUR2) << std::endl; 
}

