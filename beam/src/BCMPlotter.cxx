#include "../include/BCMPlotter.h"
//______________________________________________________________________________
BCMPlotter::BCMPlotter(const char *filePath,bool isDebug,bool enableEPICS){
   fIsDebug     = isDebug;
   fEnableEPICS = enableEPICS;  
   fTreeLeft    = nullptr; 
   fTreeSBS     = nullptr;
   fTreeEPICS   = nullptr;
   fChainLeft   = nullptr; 
   fChainSBS    = nullptr; 
   fChainEPICS  = nullptr; 
   fNEntriesLeft  = 0; 
   fNEntriesSBS   = 0; 
   fNEntriesEPICS = 0; 

   // set up chains 
   fChainLeft  = new TChain("TSLeft"); 
   fChainSBS   = new TChain("TSsbs"); 
   fChainEPICS = new TChain("E"); 

   std::string theFilePath = filePath;
   if(theFilePath.compare("NONE")!=0){
      LoadFile(filePath);
   }

   // set up variable names
   fVarName.resize(7);   
   fVarName[0] = "u1"; 
   fVarName[1] = "unew"; 
   fVarName[2] = "unser"; 
   fVarName[3] = "d1"; 
   fVarName[4] = "d3"; 
   fVarName[5] = "d10"; 
   fVarName[6] = "dnew";

   // default time variables 
   fLeftVarTime_num = "Left.104kHz_CLK.cnt";  
   fLeftVarTime_den = "Left.104kHz_CLK.rate";  
   fSBSVarTime_num  = "sbs.BBCalHi.RF.scaler";  
   fSBSVarTime_den  = "sbs.BBCalHi.RF.scalerRate"; 

   // tree variables when setting branch addresses 
   fu1r_left=0;      fu1c_left=0;
   funewr_left=0;    funewc_left=0;
   fd1r_left=0;      fd1c_left=0;
   fd3r_left=0;      fd3c_left=0;
   fd10r_left=0;     fd10c_left=0;
   fdnewr_left=0;    fdnewc_left=0;
   funserr_left=0;   funserc_left=0;
   ftime_left_num=0; ftime_left_den=0; 

   fu1r_sbs=0;      fu1c_sbs=0;
   funewr_sbs=0;    funewc_sbs=0;
   fd1r_sbs=0;      fd1c_sbs=0;
   fd3r_sbs=0;      fd3c_sbs=0;
   fd10r_sbs=0;     fd10c_sbs=0;
   fdnewr_sbs=0;    fdnewc_sbs=0;
   funserr_sbs=0;   funserc_sbs=0;
   ftime_sbs_num=0; ftime_sbs_den=0;

   fEPICSTime=0; 
   fhac_bcm_average=0;
   fIBC1H04CRCUR2=0; 
   fIPM1H04A_XPOS=0;  fIPM1H04A_YPOS=0;  
   fIPM1H04E_XPOS=0;  fIPM1H04E_YPOS=0;  

}
//______________________________________________________________________________
BCMPlotter::~BCMPlotter(){
   delete fTreeLeft;
   delete fTreeSBS;
   delete fTreeEPICS;
   delete fChainLeft; 
   delete fChainSBS;
   delete fChainEPICS;
   fVarName.clear();
}
//______________________________________________________________________________
void BCMPlotter::SetTimeVariable(const char *arm,const char *var){
   std::string armName = arm;
   if(armName.compare("Left")==0){
      fLeftVarTime_num = Form("Left.%s.cnt",var);
      fLeftVarTime_den = Form("Left.%s.rate",var);
   }else if(armName.compare("sbs")==0){
      fSBSVarTime_num = Form("sbs.%s.scaler"    ,var);
      fSBSVarTime_den = Form("sbs.%s.scalerRate",var);
   }
   // reset branch addresses 
   SetBranchAddresses(); 
}
//______________________________________________________________________________
void BCMPlotter::LoadFile(const char *filePath){
   // Attach trees for LHRS and SBS 
   TString treePath_LHRS  = Form("%s/TSLeft",filePath);
   TString treePath_SBS   = Form("%s/TSsbs" ,filePath);
   TString treePath_EPICS = Form("%s/E"     ,filePath);

   if(fIsDebug){
      std::cout << "Loading trees TSLeft, TSsbs, and E from file: " << filePath << std::endl;
   }

   fChainLeft->Add(treePath_LHRS);
   fNEntriesLeft = fChainLeft->GetEntries();
   fTreeLeft = fChainLeft->GetTree();

   fChainSBS->Add(treePath_SBS);
   fNEntriesSBS = fChainSBS->GetEntries();
   fTreeSBS  = fChainSBS->GetTree();

   if(fEnableEPICS){
      fChainEPICS->Add(treePath_EPICS);
      fNEntriesEPICS = fChainEPICS->GetEntries();
      fTreeEPICS = fChainEPICS->GetTree();
   }

   if(fIsDebug){
      std::cout << "Tree: TSLeft, addr = " << fTreeLeft  << ", NEntries = " << fNEntriesLeft  << std::endl;
      std::cout << "Tree: TSsbs , addr = " << fTreeSBS   << ", NEntries = " << fNEntriesSBS   << std::endl;
      std::cout << "Tree: E     , addr = " << fTreeEPICS << ", NEntries = " << fNEntriesEPICS << std::endl;
   }

   // now set branch addresses so we can fill vectors, histos, etc 
   SetBranchAddresses();  
}
//______________________________________________________________________________
int BCMPlotter::SetBranchAddresses(){
   // LHRS arm
   fTreeLeft->SetBranchAddress("Left.bcm.u1.cnt"    ,&fu1c_left   );
   fTreeLeft->SetBranchAddress("Left.bcm.u1.rate"   ,&fu1r_left   );
   fTreeLeft->SetBranchAddress("Left.bcm.unew.cnt"  ,&funewc_left );
   fTreeLeft->SetBranchAddress("Left.bcm.unew.rate" ,&funewr_left );
   fTreeLeft->SetBranchAddress("Left.bcm.d1.cnt"    ,&fd1c_left   );
   fTreeLeft->SetBranchAddress("Left.bcm.d1.rate"   ,&fd1r_left   );
   fTreeLeft->SetBranchAddress("Left.bcm.d3.cnt"    ,&fd3c_left   );
   fTreeLeft->SetBranchAddress("Left.bcm.d3.rate"   ,&fd3r_left   );
   fTreeLeft->SetBranchAddress("Left.bcm.d10.cnt"   ,&fd10c_left  );
   fTreeLeft->SetBranchAddress("Left.bcm.d10.rate"  ,&fd10r_left  );
   fTreeLeft->SetBranchAddress("Left.bcm.dnew.cnt"  ,&fdnewc_left );
   fTreeLeft->SetBranchAddress("Left.bcm.dnew.rate" ,&fdnewr_left );
   fTreeLeft->SetBranchAddress("Left.bcm.unser.cnt" ,&funserc_left);
   fTreeLeft->SetBranchAddress("Left.bcm.unser.rate",&funserr_left);
   fTreeLeft->SetBranchAddress(fLeftVarTime_num     ,&ftime_left_num);
   fTreeLeft->SetBranchAddress(fLeftVarTime_den     ,&ftime_left_den);
   // SBS arm
   fTreeSBS->SetBranchAddress("sbs.bcm.u1.cnt"    ,&fu1c_sbs   );
   fTreeSBS->SetBranchAddress("sbs.bcm.u1.rate"   ,&fu1r_sbs   );
   fTreeSBS->SetBranchAddress("sbs.bcm.unew.cnt"  ,&funewc_sbs );
   fTreeSBS->SetBranchAddress("sbs.bcm.unew.rate" ,&funewr_sbs );
   fTreeSBS->SetBranchAddress("sbs.bcm.d1.cnt"    ,&fd1c_sbs   );
   fTreeSBS->SetBranchAddress("sbs.bcm.d1.rate"   ,&fd1r_sbs   );
   fTreeSBS->SetBranchAddress("sbs.bcm.d3.cnt"    ,&fd3c_sbs   );
   fTreeSBS->SetBranchAddress("sbs.bcm.d3.rate"   ,&fd3r_sbs   );
   fTreeSBS->SetBranchAddress("sbs.bcm.d10.cnt"   ,&fd10c_sbs  );
   fTreeSBS->SetBranchAddress("sbs.bcm.d10.rate"  ,&fd10r_sbs  );
   fTreeSBS->SetBranchAddress("sbs.bcm.dnew.cnt"  ,&fdnewc_sbs );
   fTreeSBS->SetBranchAddress("sbs.bcm.dnew.rate" ,&fdnewr_sbs );
   fTreeSBS->SetBranchAddress("sbs.bcm.unser.cnt" ,&funserc_sbs);
   fTreeSBS->SetBranchAddress("sbs.bcm.unser.rate",&funserr_sbs);
   fTreeSBS->SetBranchAddress(fSBSVarTime_num     ,&ftime_sbs_num);
   fTreeSBS->SetBranchAddress(fSBSVarTime_den     ,&ftime_sbs_den);
   // EPICS 
   if(fEnableEPICS){
      fTreeEPICS->SetBranchAddress("hac_bcm_average",&fhac_bcm_average);
      fTreeEPICS->SetBranchAddress("IPM1H04A.XPOS"  ,&fIPM1H04A_XPOS  );
      fTreeEPICS->SetBranchAddress("IPM1H04A.YPOS"  ,&fIPM1H04A_YPOS  );
      fTreeEPICS->SetBranchAddress("IPM1H04E.XPOS"  ,&fIPM1H04E_XPOS  );
      fTreeEPICS->SetBranchAddress("IPM1H04E.YPOS"  ,&fIPM1H04E_YPOS  );
      fTreeEPICS->SetBranchAddress("IBC1H04CRCUR2"  ,&fIBC1H04CRCUR2  );
      fTreeEPICS->SetBranchAddress("timestamp"      ,&fEPICSTime      );
   }
   return 0;
}
//______________________________________________________________________________
TH1F * BCMPlotter::GetTH1F(const char *h_name,const char *h_range,
                           const char *arm,const char *var_name,const char *type){
   // construct a TH1F 
   std::string armName   = arm;
   std::string varName   = var_name;
   TString fullVarName   = Form("%s.bcm.%s.%s",armName.c_str(),varName.c_str(),type); 
   TString hName         = h_name; 
   TString hNameAndRange = Form("%s%s",h_name,h_range); 

   int NV = fVarName.size(); 

   int found = CheckVariable(arm,var_name);

   if(found==1){
      // found the variable we want to plot 
      if(armName.compare("Left")==0){
         fTreeLeft->Project(hNameAndRange,fullVarName,"","");
      }else{
         fTreeSBS->Project(hNameAndRange,fullVarName,"","");
      } 
   }else{
      std::cout << "[BCMPlotter::GetTH1F]: ERROR!  Invalid variable name = " << varName << std::endl;
      return 0;
   }

   // get the created histo and return it
   TH1F *h = (TH1F *)gDirectory->Get(hName); 
   return h; 
}
//______________________________________________________________________________
TH2F * BCMPlotter::GetTH2F(const char *h_name,const char *h_range,
                           const char *arm,const char *xAxis,const char *yAxis,const char *type){

   // construct a TH1F 
   std::string armName   = arm;
   std::string varName   = yAxis;  

   // have to carefully parse the x axis here (usually time)  
   TString xAxisName;
   if(armName.compare("Left")==0){
      xAxisName = Form("Left.%s.cnt/Left.%s.rate",xAxis,xAxis);
   }else if(armName.compare("sbs")==0){
      xAxisName = Form("sbs.%s.scaler/sbs.%s.scalerRate",xAxis,xAxis);
   } 

   TString yAxisName     = Form("%s.bcm.%s.%s",arm,yAxis,type);
   TString fullVarName   = Form("%s:%s",yAxisName.Data(),xAxisName.Data()); 
   TString hName         = h_name; 
   TString hNameAndRange = Form("%s%s",h_name,h_range); 

   int NV = fVarName.size(); 

   int found = CheckVariable(arm,yAxis);

   if(found==1){
      // found the variable we want to plot 
      if(armName.compare("Left")==0){
         fTreeLeft->Project(hNameAndRange,fullVarName,"","");
      }else{
         fTreeSBS->Project(hNameAndRange,fullVarName,"","");
      } 
   }else{
      std::cout << "[BCMPlotter::GetTH2F]: ERROR!  Invalid variable name = " << varName << std::endl;
      return 0;
   }

   // get the created histo and return it
   TH2F *h = (TH2F *)gDirectory->Get(hName); 
   return h;
}
//______________________________________________________________________________
int BCMPlotter::CheckVariable(const char *arm,const char *var){
   // make sure the variable exists
   std::string armName = arm;
   std::string varName = var; 

   int found = 0;
   int NV    = fVarName.size(); 

   if(armName.compare("Left")==0 || armName.compare("sbs")==0){
      for(int i=0;i<NV;i++){
         if(varName.compare(fVarName[i])==0) found++;           
      }
   }else{
      std::cout << "[BCMPlotter::CheckVariable]: ERROR! Invalid arm name = " << armName << std::endl;
      return 0;
   }

   if(found!=1){
      std::cout << "[BCMPlotter::CheckVariable]: ERROR!  Invalid variable name = " << varName << std::endl;
   } 

   return found;
}
//______________________________________________________________________________
TGraph * BCMPlotter::GetTGraph(const char *arm,const char *xAxis,const char *yAxis,const char *type){
   // get a TGraph object 
   std::string ARM = arm; 
   TString xAxisName = Form("%s",xAxis);
   TString yAxisName;
   if(ARM.compare("E")==0){
      yAxisName = Form("%s",yAxis); 
   }else{
      yAxisName = Form("%s.bcm.%s.%s",arm,yAxis,type);
   }

   if(fIsDebug) std::cout << xAxisName << " " << yAxisName << std::endl;

   std::vector<double> x,y; 
   GetVector(arm,xAxisName.Data(),x); 
   GetVector(arm,yAxisName.Data(),y);

   const int N = x.size(); 
   TGraph *g = new TGraph(N,&x[0],&y[0]); 
   return g; 
}
//______________________________________________________________________________
int BCMPlotter::GetVector(const char *arm,const char *var,std::vector<double> &v){
   // fill a vector with the variable 

   std::string armName = arm;
   std::string varName = var;

   if(fIsDebug) std::cout << "arm = " << arm << ", var = " << var << std::endl;

   int NN=0;
   if(armName.compare("Left")==0) NN = fNEntriesLeft; 
   if(armName.compare("sbs")==0)  NN = fNEntriesSBS; 
   if(armName.compare("E")==0)    NN = fNEntriesEPICS; 

   double val=0,time=0;
   for(int i=0;i<NN;i++){
      if(armName.compare("Left")==0){
         fTreeLeft->GetEntry(i);
	 if(ftime_left_den!=0) time = ftime_left_num/ftime_left_den;
	 if(varName.compare("time")==0) val = time;
	 if(varName.compare("Left.bcm.u1.cnt")==0     ) val = fu1c_left;
	 if(varName.compare("Left.bcm.unew.cnt")==0   ) val = funewc_left;
	 if(varName.compare("Left.bcm.d1.cnt")==0     ) val = fd1c_left;
	 if(varName.compare("Left.bcm.d3.cnt")==0     ) val = fd3c_left;
	 if(varName.compare("Left.bcm.d10.cnt")==0    ) val = fd10c_left;
	 if(varName.compare("Left.bcm.dnew.cnt")==0   ) val = fdnewc_left;
	 if(varName.compare("Left.bcm.unser.cnt")==0  ) val = funserc_left;
	 if(varName.compare("Left.bcm.u1.rate")==0    ) val = fu1r_left;
	 if(varName.compare("Left.bcm.unew.rate")==0  ) val = funewr_left;
	 if(varName.compare("Left.bcm.d1.rate")==0    ) val = fd1r_left;
	 if(varName.compare("Left.bcm.d3.rate")==0    ) val = fd3r_left;
	 if(varName.compare("Left.bcm.d10.rate")==0   ) val = fd10r_left;
	 if(varName.compare("Left.bcm.dnew.rate")==0  ) val = fdnewr_left;
	 if(varName.compare("Left.bcm.unser.rate")==0 ) val = funserr_left;
      }else if(armName.compare("sbs")==0){
         fTreeSBS->GetEntry(i);
	 if(ftime_sbs_den!=0) time = ftime_sbs_num/ftime_sbs_den;
	 if(varName.compare("time")==0) val = time;
	 if(varName.compare("sbs.bcm.u1.cnt")==0     ) val = fu1c_sbs;
	 if(varName.compare("sbs.bcm.unew.cnt")==0   ) val = funewc_sbs;
	 if(varName.compare("sbs.bcm.d1.cnt")==0     ) val = fd1c_sbs;
	 if(varName.compare("sbs.bcm.d3.cnt")==0     ) val = fd3c_sbs;
	 if(varName.compare("sbs.bcm.d10.cnt")==0    ) val = fd10c_sbs;
	 if(varName.compare("sbs.bcm.dnew.cnt")==0   ) val = fdnewc_sbs;
	 if(varName.compare("sbs.bcm.unser.cnt")==0  ) val = funserc_sbs;
	 if(varName.compare("sbs.bcm.u1.rate")==0    ) val = fu1r_sbs;
	 if(varName.compare("sbs.bcm.unew.rate")==0  ) val = funewr_sbs;
	 if(varName.compare("sbs.bcm.d1.rate")==0    ) val = fd1r_sbs;
	 if(varName.compare("sbs.bcm.d3.rate")==0    ) val = fd3r_sbs;
	 if(varName.compare("sbs.bcm.d10.rate")==0   ) val = fd10r_sbs;
	 if(varName.compare("sbs.bcm.dnew.rate")==0  ) val = fdnewr_sbs;
	 if(varName.compare("sbs.bcm.unser.rate")==0 ) val = funserr_sbs;
	 // std::cout << Form("%d val = %.3lf, u1r = %.3lf, d1r = %.3lf",i,val,fu1r_sbs,fd1r_sbs) << std::endl;
      }else if(armName.compare("E")==0){
	 fTreeEPICS->GetEntry(i); 
	 if(varName.compare("time")==0)            val = fEPICSTime; 
	 if(varName.compare("IPM1H04A.XPOS")==0)   val = fIPM1H04A_XPOS; 
	 if(varName.compare("IPM1H04A.YPOS")==0)   val = fIPM1H04A_YPOS; 
	 if(varName.compare("IPM1H04E.XPOS")==0)   val = fIPM1H04E_XPOS; 
	 if(varName.compare("IPM1H04E.YPOS")==0)   val = fIPM1H04E_YPOS; 
	 if(varName.compare("hac_bcm_average")==0) val = fhac_bcm_average; 
	 if(varName.compare("IBC1H04CRCUR2")==0)   val = fIBC1H04CRCUR2; 
      }
      if(fIsDebug) std::cout << Form("%d time = %.3lf, u1r = %.3lf, d1r = %.3lf",i,time,fu1r_sbs,fd1r_sbs) << std::endl;
      v.push_back(val);
      val = 0;
      time = 0;
   }
   return 0;
}
