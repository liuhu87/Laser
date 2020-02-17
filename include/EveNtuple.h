//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Feb 12 13:04:16 2020 by ROOT version 5.34/38
// from TTree laserevent/info of mean led events in 30s
// found on file: ES.30313.FULL.WFCTA01.20191121202743.005.cal.root
//////////////////////////////////////////////////////////

#ifndef EveNtuple_h
#define EveNtuple_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>

// Header file for the classes stored in the TTree if any.
#include <vector>
#include <vector>
using std::vector;

// Fixed size dimensions of array or collections stored in the TTree if any.

class EveNtuple : public TSelector {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain

   // Declaration of leaf types
   Long64_t        rabbitTime;
   Double_t        rabbittime;
   vector<int>     *isipm;
   vector<double>  *sipmpe;
   vector<double>  *sipmt;

   // List of branches
   TBranch        *b_rabbitTime;   //!
   TBranch        *b_rabbittime;   //!
   TBranch        *b_isipm;   //!
   TBranch        *b_sipmpe;   //!
   TBranch        *b_sipmt;   //!

   EveNtuple(TTree * /*tree*/ =0) : fChain(0) { }
   virtual ~EveNtuple() { }
   virtual Int_t   Version() const { return 2; }
   virtual void    Begin(TTree *tree);
   virtual void    SlaveBegin(TTree *tree);
   virtual void    Init(TTree *tree);
   virtual Bool_t  Notify();
   virtual Bool_t  Process(Long64_t entry);
   virtual Int_t   GetEntry(Long64_t entry, Int_t getall = 0) { return fChain ? fChain->GetTree()->GetEntry(entry, getall) : 0; }
   virtual void    SetOption(const char *option) { fOption = option; }
   virtual void    SetObject(TObject *obj) { fObject = obj; }
   virtual void    SetInputList(TList *input) { fInput = input; }
   virtual TList  *GetOutputList() const { return fOutput; }
   virtual void    SlaveTerminate();
   virtual void    Terminate();

   ClassDef(EveNtuple,0);
};

#endif

#ifdef EveNtuple_cxx
void EveNtuple::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   isipm = 0;
   sipmpe = 0;
   sipmt = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("rabbitTime", &rabbitTime, &b_rabbitTime);
   fChain->SetBranchAddress("rabbittime", &rabbittime, &b_rabbittime);
   fChain->SetBranchAddress("isipm", &isipm, &b_isipm);
   fChain->SetBranchAddress("sipmpe", &sipmpe, &b_sipmpe);
   fChain->SetBranchAddress("sipmt", &sipmt, &b_sipmt);
}

Bool_t EveNtuple::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

#endif // #ifdef EveNtuple_cxx
