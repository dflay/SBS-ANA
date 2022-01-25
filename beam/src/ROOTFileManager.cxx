#include "../include/ROOTFileManager.h"
//______________________________________________________________________________
namespace util {
   //______________________________________________________________________________
   ROOTFileManager::ROOTFileManager(){

   }
   //______________________________________________________________________________
   ROOTFileManager::~ROOTFileManager(){

   }
   //______________________________________________________________________________
   int ROOTFileManager::Clear(){
      fData->ClearData();
   }
   //______________________________________________________________________________
   int ROOTFileManager::CheckFile(const char *filePath){
      TFile *myFile = new TFile(filePath);
      Long64_t bytesRead = myFile->GetBytesRead();
      int rc=1;
      if(bytesRead!=0){
	 // file has non-zero size
	 rc = 0;
      }
      myFile->Close();
      return rc;
   }
   //______________________________________________________________________________
   int ROOTFileManager::LoadFile(rootData_t metaData){
      // load data from a ROOT file given the parameters defined in the rootData struct
      // data is loaded to the local CSV manager
      int rc = CheckFile(metaData.fileName.Data());
      if(rc!=0){
	 std::cout << "[ROOTFileManager::LoadFile]: Cannot open the file " << metaData.fileName << std::endl;
	 return rc;
      }

      // TODO: determine the leaf structure 
      std::vector<std::string> leafStructure; 

      // load data from the tree
      rc = LoadDataFromTree(metaData.fileName.Data(),metaData.treeName.Data(),leafStructure);  
  
      return rc;
   }
   //______________________________________________________________________________
   int ROOTFileManager::LoadDataFromTree(const char *filePath,const char *treeName,
                                         std::vector<std::string> leafStructure){
      // load data from a tree
      // create the TChain and attach to the tree 
      TChain *ch = nullptr;
      ch = new TChain(treeName);
      ch->Add(filePath);

      // error checking
      if(ch==nullptr){
	 return 1;
      }

      TTree *aTree = ch->GetTree();
      if(aTree==nullptr) return 1;
       
      // passed all tests, now populate the fData container  
      const int NN   = ch->GetEntries();
      const int NVAR = leafStructure.size();

      // TODO: set branch addresses in the ROOT file

      // initialize the size of the table
      fData->InitTable(NN,NVAR); // NN = num rows, NVAR = num columns  

      // TODO: populate the fData container
      for(int i=0;i<NN;i++){

      }
     
      return 0;
   }
} // ::util
