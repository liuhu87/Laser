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
#include "TH2F.h"
#include "TGraphErrors.h"
#include "RotateDB.h"
#include "StatusDB.h"
#include "common.h"

extern int iTel;
extern bool doclean;
extern int timefirst;
extern int timelast;
extern TFile* outfile;
WFCTAEvent* pev=0;
extern const char* outname;
char OutType[20];
int nplot=0;

bool process1=true;
bool process2=true;
bool UseGraph=true;

TH2F* htemp_sgl[1024];
TH2F* htemp_ave[1024];
TH2F* hsum_temp_ave;

int piEvent=1000;
int nfile=0;
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
   WFCTAEvent::npetrigger=50;
   WFCTAEvent::nfiretrigger=3;

   RotateDB::jdebug=0;
   RotateDB::ntotmin=5;
   RotateDB::nsidemin=2;
   RotateDB::phimargin=10;
   RotateDB::ccmargin=1.;

   double xbins[100000];
   int nxbin=CommonTools::GetBins(timefirst,timelast,600.,xbins);
   printf("Total %d time bins\n",nxbin);
   if(nxbin==100000-1){
      printf("BinError,Please Increase BinWidth, Exiting...\n");
      return;
   }

   // create histogram to save results
   for(int ii=0;ii<1024;ii++){
      htemp_sgl[ii]=new TH2F(Form("npe_temp_sgl_sipm%d",ii),";Temperature [^{o}C];Npe",140,-40,30,500,0,1000);
      htemp_ave[ii]=new TH2F(Form("npe_temp_ave_sipm%d",ii),";Temperature [^{o}C];Npe",140,-40,30,500,0,1000);
   }
   hsum_temp_ave=new TH2F(Form("sum_npe_temp_ave"),";Temperature [^{o}C];Npe",140,-40,30,500,1.0e3,1.0e6);
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
   //if(pev->iEvent<piEvent){
   //   printf("Processing file %d of %d name=%s...\n",nfile+first,last+1,pev->GetFileName());
   //   nfile++;
   //}
   //piEvent=pev->iEvent;


   pev->iTel=iTel;
   pev->rabbitTime=rabbitTime;
   pev->rabbittime=rabbittime;
   pev->iSiPM.clear();
   pev->AdcH.clear();
   pev->AdcL.clear();
   pev->SatH.clear();
   pev->PeakPosH.clear();
   int size=isipm->size();
   for(int ii=0;ii<size;ii++){
      pev->iSiPM.push_back(isipm->at(ii));
      pev->AdcH.push_back(sipmpe->at(ii)*WFCTAMCEvent::fAmpHig);
      pev->AdcL.push_back(sipmpe->at(ii)*WFCTAMCEvent::fAmpLow);
      pev->PeakPosH.push_back((short)(sipmt->at(ii)));
      pev->SatH.push_back(false);
   }

   RotateDB* pr=RotateDB::GetHead();
   int irot=pr->GetLi(double(pev->rabbittime));
   int itel=pr->GetTi(pev->iTel);
   if((entry%100)==0) printf("entry=%d iTel=%d event=%ld time={%d,%lf}\n",entry,pev->iTel,pev->iEvent,pev->rabbitTime,pev->rabbittime);

   //printf("Entry=%ld Time={%ld+%.8lf} {%ld+%.8lf} size=%d\n",entry,rabbitTime,rabbittime*2.0e-8,pev->rabbitTime,pev->rabbittime*2.0e-8,size);
   //for(int ii=0;ii<size;ii++){
   //   printf("ii=%d isipm=%d sipmpe=%lf sipmt=%lf\n",ii,isipm->at(ii),sipmpe->at(ii),sipmt->at(ii));
   //   printf("ii=%d isipm=%d sipmpe=%lf sipmt=%lf\n",ii,pev->iSiPM.at(ii),pev->AdcH.at(ii),pev->PeakPosH.at(ii)*1.);
   //}
   //printf("\n");

   //select laser event
   int index=-1;

   //index=pr->GetEleAzi(pev);
   //if(index<=0) return kTRUE;
   //if((index/10)!=1) return kTRUE;

   if(irot<0) return kTRUE;

   index=pr->LaserIsFine(pev);
   if(doclean) {if(index<=0) return kTRUE;}

   int itype=((index%100)/10);
   int iangle=(index%10);

   printf("LaserEvent: entry=%d time={%d+%.8lf} index=%d\n",entry,pev->rabbitTime,pev->rabbittime*2.0e-8,index);
   if(index!=221) return kTRUE;

   StatusDB::GetHead()->GetReadbackTime(pev->iTel,pev->rabbitTime);
   double temperature[1024];
   double temp_ave=0;
   int ntemp=0;
   for(int ii=0;ii<1024;ii++){
      double temp=StatusDB::GetHead()->PreTemp[ii];
      if(temp==0) continue;
      temperature[ii]=temp;
      temp_ave+=temp;
      ntemp++;
   }
   if(ntemp>0) temp_ave/=ntemp;

   double npe_sum=0;
   for(int ii=0;ii<size;ii++){
      int sipm0=pev->iSiPM.at(ii);
      double npe=pev->AdcH.at(ii)/WFCTAMCEvent::fAmpHig;
      npe_sum+=npe;
      htemp_sgl[sipm0]->Fill(temperature[ii],npe);
      htemp_ave[sipm0]->Fill(temp_ave,npe);
   }
   hsum_temp_ave->Fill(temp_ave,npe_sum);

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
   if(outfile){
      outfile->cd();
      for(int ii=0;ii<1024;ii++){
         if(htemp_sgl[ii]->Integral()>0) htemp_sgl[ii]->Write();
         if(htemp_ave[ii]->Integral()>0) htemp_ave[ii]->Write();
      }
      hsum_temp_ave->Write();
   }
}
