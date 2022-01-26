#include "../include/ROOTFileManager.h"
//______________________________________________________________________________
namespace util {
   //______________________________________________________________________________
   ROOTFileManager::ROOTFileManager(){
      fIsDebug = false;
   }
   //______________________________________________________________________________
   ROOTFileManager::~ROOTFileManager(){
      const int N = fData.size();
      for(int i=0;i<N;i++){
	 delete fData[i];
	 fBranchName[i].clear(); 
	 fBufSize[i].clear(); 
      } 
      fData.clear();
      fTreeName.clear();
   }
   //______________________________________________________________________________
   int ROOTFileManager::Clear(){
      // clear the data structure
      // NOTE: We are not *deleting* the vector, just setting everything 
      // back to zero effectively 
      const int N = fData.size();
      for(int i=0;i<N;i++){
	 fData[i]->ClearData();
	 fBranchName[i].clear(); 
	 fBufSize[i].clear(); 
      }
      return 0;
   }
   //______________________________________________________________________________
   int ROOTFileManager::LoadFileStructure(const char *inpath){
      // load in the list of trees and branches
      CSVManager *csv = new CSVManager();
      csv->ReadFile(inpath,true);
 
      std::vector<std::string> tree,branch,bufSize; 
      csv->GetColumn_byName_str("treeName"  ,tree   );
      csv->GetColumn_byName_str("branchName",branch );
      csv->GetColumn_byName_str("bufSize"   ,bufSize);
      delete csv; 

      // find number of branches
      std::vector<char> buf; 
      std::vector<std::string> bb;  
      std::string theTree_prev=tree[0]; 
      const int N = branch.size();
      for(int i=1;i<N;i++){
         if(tree[i].compare(theTree_prev)!=0){
            // new tree name
            fTreeName.push_back(theTree_prev);
            // store branches and buffer sizes  
            fBranchName.push_back(bb);
            fBufSize.push_back(buf);
            // clear branch history 
            bb.clear(); 
            buf.clear(); 
         }else{
            // for a given tree name, save the branches 
            bb.push_back(branch[i]); 
            buf.push_back((char)bufSize[i][0]);  
         }        
         theTree_prev = tree[i]; 
      } 
 
      // and the last one that's not processed 
      // new tree name
      fTreeName.push_back(theTree_prev);
      // store branches and buffer sizes  
      fBranchName.push_back(bb);
      fBufSize.push_back(buf);
      // clear branch history 
      bb.clear();
      buf.clear();

      int NB = 0; 
      int NT = fTreeName.size();
      // if(fIsDebug){
         std::cout << "[ROOTFileManager::LoadFileStructure]: Found " << NT << " trees: " << std::endl; 
         for(int i=0;i<NT;i++){
            std::cout << Form("Tree: %s",fTreeName[i].c_str()) << std::endl;
            NB = fBranchName[i].size();
            for(int j=0;j<NB;j++){
               std::cout << Form("   Branch: %s, bufSize = %c",fBranchName[i][j].c_str(),fBufSize[i][j]) << std::endl;
            }
         }
      // }
      return 0;
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

      rc = LoadFileStructure(metaData.structurePath.Data()); 
      if(rc!=0){
	 std::cout << "[ROOTFileManager::LoadFile]: Cannot open the file " << metaData.structurePath << std::endl;
	 return rc;
      }

      // load data from the tree
      const int NT = fTreeName.size();
      for(int i=0;i<NT;i++){
	 rc = LoadDataFromTree(metaData.fileName.Data(),fTreeName[i].c_str(),fBranchName[i],fBufSize[i]);
      }
  
      return rc;
   }
   //______________________________________________________________________________
   int ROOTFileManager::LoadDataFromTree(const char *filePath,const char *treeName,
                                         std::vector<std::string> branch,
                                         std::vector<char> bufSize){
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
      const int NN = ch->GetEntries();
      const int NB = branch.size();

      std::vector<int> var_i;      // 32-bit signed integer
      std::vector<float> var_f;    // 32-bit floating point 
      std::vector<double> var_d;   // 64-bit floating point 
      for(int i=0;i<NB;i++){
	 if(bufSize[i]=='F') aTree->SetBranchAddress(branch[i].c_str(),&var_d[i]);
	 if(bufSize[i]=='I') aTree->SetBranchAddress(branch[i].c_str(),&var_i[i]);
      }

      // create a new CSV object 
      CSVManager *data = new CSVManager();
      data->InitTable(NN,NB);  // init size of table; NN = num rows, NVAR = num columns 
      data->SetHeader(branch); // set the header using the branch names   

      // FIXME: How to handle different data types?
      for(int i=0;i<NN;i++){
	 aTree->GetEntry(i);
	 for(int j=0;j<NB;j++){
	    if(bufSize[i]=='F') data->SetElement<double>(i,j,var_d[j]);
	    if(bufSize[i]=='I') data->SetElement<int>(i,j,var_i[j]);
         }
      }

      // push-back on the fData vector 
      fData.push_back(data); 

      // TODO: do we need to delete this?
      // delete data; 

      return 0;
   }
} // ::util
