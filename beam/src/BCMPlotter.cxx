#include "../include/BCMPlotter.h"
//______________________________________________________________________________
BCMPlotter::BCMPlotter(const char *filePath){
   fIsDebug  = false; 
   fTreeLeft = nullptr; 
   fTreeSBS  = nullptr;
   fNEntries = 0; 

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

}
//______________________________________________________________________________
BCMPlotter::~BCMPlotter(){
   delete fTreeLeft;
   delete fTreeSBS;
   fVarName.clear();
}
//______________________________________________________________________________
void BCMPlotter::LoadFile(const char *filePath){

   TString treePath_LHRS = Form("%s/TSLeft",filePath);
   TString treePath_SBS  = Form("%s/TSsbs" ,filePath);

   if(fIsDebug){
      std::cout << "Loading trees TSLeft and TSsbs from file: " << filePath << std::endl;
   }

   TChain *chL = new TChain("TSLeft");
   chL->Add(treePath_LHRS);
   fNEntries = chL->GetEntries();
   fTreeLeft = chL->GetTree();

   TChain *chS = new TChain("TSsbs");
   chS->Add(treePath_SBS);
   fNEntries = chS->GetEntries();
   fTreeSBS = chS->GetTree();

   if(fIsDebug) std::cout << "NEntries = " << fNEntries << ", Tree addresses: TSLeft = " << fTreeLeft << " TSsbs = " << fTreeSBS << std::endl;

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

   int found=0;

   if(armName.compare("Left")==0 || armName.compare("sbs")==0){
      // make sure the variable exists
      for(int i=0;i<NV;i++){
         if(varName.compare(fVarName[i])==0) found++;           
      }
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
   }else{
      std::cout << "[BCMPlotter::GetTH1F]: ERROR! Invalid arm name = " << armName << std::endl;
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

   // have to carefully parse the time axis here 
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

   int found=0;

   if(armName.compare("Left")==0 || armName.compare("sbs")==0){
      // make sure the variable exists
      for(int i=0;i<NV;i++){
         if(varName.compare(fVarName[i])==0) found++;           
      }
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
   }else{
      std::cout << "[BCMPlotter::GetTH1F]: ERROR! Invalid arm name = " << armName << std::endl;
      return 0;
   }
   // get the created histo and return it
   TH2F *h = (TH2F *)gDirectory->Get(hName); 
   return h;
}
