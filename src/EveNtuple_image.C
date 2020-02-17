#define EveNtuple_cxx
// The class definition in EveNtuple.h has been generated automatically
// by the ROOT utility TTree::MakeSelector(). This class is derived
// from the ROOT class TSelector. For more information on the TSelector
// framework see $ROOTSYS/README/README.SELECTOR or the ROOT User Manual.

// The following methods are defined in this file:
//    Begin():        called every time a loop on the tree starts,
//                    a convenient place to create your histograms.
//    SlaveBegin():   called after Begin(), when on PROOF called only on the
//                    slave servers.
//    Process():      called for each event, in this function you decide what
//                    to read and fill your histograms.
//    SlaveTerminate: called at the end of the loop on the tree, when on PROOF
//                    called only on the slave servers.
//    Terminate():    called at the end of the loop on the tree,
//                    a convenient place to draw/fit your histograms.
//
// To use this file, try the following session on your Tree T:
//
// Root > T->Process("EveNtuple.C")
// Root > T->Process("EveNtuple.C","some options")
// Root > T->Process("EveNtuple.C+")
//

#include "EveNtuple.h"
#include <TH2.h>
#include <TStyle.h>

#include "WFCTAEvent.h"
#include "TCanvas.h"
#include "RotateDB.h"

extern int iTel;
extern bool doclean;
WFCTAEvent* pev=0;
extern const char* outname;
char OutType[20];
int nplot=0;

void EveNtuple::Begin(TTree * /*tree*/)
{
   // The Begin() function is called at the start of the query.
   // When running with PROOF Begin() is only called on the client.
   // The tree argument is deprecated (on PROOF 0 is passed).

   TString option = GetOption();

   if(!pev) pev=new WFCTAEvent();
   int length=strlen(outname);
   int iloc=-1;
   for(int ii=length-1;ii>=0;ii--){
      if(outname[ii]=='.'){ iloc=ii; break;}
   }
   for(int ii=0;ii<length;ii++){
      if(ii>iloc) OutType[ii-iloc-1]=outname[ii];
   }
   OutType[length-1-iloc]='\0';

   WFCTAEvent::DoDraw=false;
   RotateDB::jdebug=0;
   RotateDB::ntotmin=5;
   RotateDB::nsidemin=2;
}

void EveNtuple::SlaveBegin(TTree * /*tree*/)
{
   // The SlaveBegin() function is called after the Begin() function.
   // When running with PROOF SlaveBegin() is called on each slave server.
   // The tree argument is deprecated (on PROOF 0 is passed).

   TString option = GetOption();

}

Bool_t EveNtuple::Process(Long64_t entry)
{
   // The Process() function is called for each entry in the tree (or possibly
   // keyed object in the case of PROOF) to be processed. The entry argument
   // specifies which entry in the currently loaded tree is to be processed.
   // It can be passed to either EveNtuple::GetEntry() or TBranch::GetEntry()
   // to read either all or the required parts of the data. When processing
   // keyed objects with PROOF, the object is already loaded and is available
   // via the fObject pointer.
   //
   // This function should contain the "body" of the analysis. It can contain
   // simple or elaborate selection criteria, run algorithms on the data
   // of the event and typically fill histograms.
   //
   // The processing can be stopped by calling Abort().
   //
   // Use fStatus to set the return value of TTree::Process().
   //
   // The return value is currently not used.

   GetEntry(entry);

   if(entry==0) printf("Processing new file %s(entry=%ld)\n",((TChain*)fChain)->GetFile()->GetName(),fChain->GetEntries());
   if((entry%100)==0) printf("Processing Entry %ld ...\n",entry);

   pev->iTel=iTel;
   pev->rabbitTime=rabbitTime;
   pev->rabbittime=rabbittime;
   pev->iSiPM.clear();
   pev->AdcH.clear();
   pev->AdcL.clear();
   pev->PeakPosH.clear();
   int size=isipm->size();
   for(int ii=0;ii<size;ii++){
      pev->iSiPM.push_back(isipm->at(ii));
      pev->AdcH.push_back(sipmpe->at(ii)*WFCTAMCEvent::fAmpHig);
      pev->AdcL.push_back(sipmpe->at(ii)*WFCTAMCEvent::fAmpLow);
      pev->PeakPosH.push_back((short)(sipmt->at(ii)));
   }

   //printf("Entry=%ld Time={%ld+%.8lf} {%ld+%.8lf} size=%d\n",entry,rabbitTime,rabbittime*2.0e-8,pev->rabbitTime,pev->rabbittime*2.0e-8,size);
   //for(int ii=0;ii<size;ii++){
   //   printf("ii=%d isipm=%d sipmpe=%lf sipmt=%lf\n",ii,isipm->at(ii),sipmpe->at(ii),sipmt->at(ii));
   //   printf("ii=%d isipm=%d sipmpe=%lf sipmt=%lf\n",ii,pev->iSiPM.at(ii),pev->AdcH.at(ii),pev->PeakPosH.at(ii)*1.);
   //}
   //printf("\n");

   //select laser event
   RotateDB* pr=RotateDB::GetHead();
   int index=-1;

   //index=pr->GetEleAzi(pev);
   //if(index<=0) return kTRUE;
   //if((index/10)!=1) return kTRUE;

   if(pr->GetLi((double)pev->rabbittime)<0) return kTRUE;

   index=pr->LaserIsFine(pev);
   if(doclean) {if(index<=0) return kTRUE;}

   printf("LaserEvent: entry=%d time={%d+%.8lf} index=%d\n",entry,pev->rabbitTime,pev->rabbittime*2.0e-8,index);

   if(strstr(OutType,"root")){
   }
   else{
      TCanvas* cc=new TCanvas();
      pev->Draw(3,"colz",false);
      cc->Print(Form("%s%s",outname,nplot==0?"(":""),OutType);
      nplot++;
   }

   return kTRUE;
}

void EveNtuple::SlaveTerminate()
{
   // The SlaveTerminate() function is called after all entries or objects
   // have been processed. When running with PROOF SlaveTerminate() is called
   // on each slave server.

}

void EveNtuple::Terminate()
{
   // The Terminate() function is the last function to be called during
   // a query. It always runs on the client, it can be used to present
   // the results graphically or save the results to file.

   if(pev) delete pev;
}
