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
bool UseHist=true;

   const int nangle1=4;
   TGraphErrors* glist1[10][20][nangle1];
   TH2F* hlist1[10][20][4];
   TH1F* hcount1[10][20][4];
   const int nangle2=6;
   TGraphErrors* glist2[10][20][nangle2];
   TH2F* hlist2[10][20][nangle2];
   TH1F* hcount2[10][20][nangle2];
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
   for(int irot=0;irot<10;irot++){
      for(int itel=0;itel<20;itel++){
         for(int iazi=0;iazi<nangle1;iazi++){
            glist1[irot][itel][iazi]=0;
            hlist1[irot][itel][iazi]=0;
            hcount1[irot][itel][iazi]=0;
            if(irot>=RotateDB::nrot) continue;
            if(itel>=RotateDB::ntel) continue;
            if(!process1) continue;
            if(iazi==0&&(RotateDB::telindex[itel]!=5&&RotateDB::telindex[itel]!=6)) continue;
            if(iazi==1&&(RotateDB::telindex[itel]!=4&&RotateDB::telindex[itel]!=5)) continue;
            if(iazi==2&&(RotateDB::telindex[itel]!=3&&RotateDB::telindex[itel]!=4)) continue;
            if(iazi==3&&(RotateDB::telindex[itel]!=1&&RotateDB::telindex[itel]!=2&&RotateDB::telindex[itel]!=3)) continue;
            if(UseGraph){
               glist1[irot][itel][iazi]=new TGraphErrors();
               glist1[irot][itel][iazi]->SetName(Form("gRot%d_Tel%d_Azi%d",RotateDB::rotindex[irot],RotateDB::telindex[itel],iazi+1));
               glist1[irot][itel][iazi]->GetXaxis()->SetTitle("Time");
               glist1[irot][itel][iazi]->GetXaxis()->SetTimeDisplay(1);
               glist1[irot][itel][iazi]->GetXaxis()->SetNdivisions(-203);
               glist1[irot][itel][iazi]->GetXaxis()->SetTimeFormat("%Mm/%Hh/%d/%m/%Y%F1970-01-01 00:00:00s0");
               glist1[irot][itel][iazi]->GetYaxis()->SetTitle("Total Signal [pe]");
            }
            if(UseHist){
               hlist1[irot][itel][iazi]=new TH2F(Form("Rot%d_Tel%d_Azi%d",RotateDB::rotindex[irot],RotateDB::telindex[itel],iazi+1),";Time;Long Axis [degree]",nxbin,xbins,48,-12.,12.);
               hlist1[irot][itel][iazi]->GetXaxis()->SetTimeDisplay(1);
               hcount1[irot][itel][iazi]=new TH1F(Form("NAve_Rot%d_Tel%d_Azi%d",RotateDB::rotindex[irot],RotateDB::telindex[itel],iazi+1),";Time;Events",nxbin,xbins);
               hcount1[irot][itel][iazi]->GetXaxis()->SetTimeDisplay(1);
            }
         }
      }
   }

   for(int irot=0;irot<10;irot++){
      for(int itel=0;itel<20;itel++){
         for(int iele=0;iele<nangle2;iele++){
            glist2[irot][itel][iele]=0;
            hlist2[irot][itel][iele]=0;
            hcount2[irot][itel][iele]=0;
            if(!process2) continue;
            if(irot>=RotateDB::nrot) continue;
            if(itel>=RotateDB::ntel) continue;
            if(UseGraph){
               glist2[irot][itel][iele]=new TGraphErrors();
               glist2[irot][itel][iele]->SetName(Form("gRot%d_Tel%d_Ele%d",RotateDB::rotindex[irot],RotateDB::telindex[itel],iele+1));
               glist2[irot][itel][iele]->GetXaxis()->SetTitle("Time");
               glist2[irot][itel][iele]->GetXaxis()->SetTimeDisplay(1);
               glist2[irot][itel][iele]->GetXaxis()->SetNdivisions(-203);
               glist2[irot][itel][iele]->GetXaxis()->SetTimeFormat("%Mm/%Hh/%d/%m/%Y%F1970-01-01 00:00:00s0");
               glist2[irot][itel][iele]->GetYaxis()->SetTitle("Total Signal [pe]");
            }
            if(UseHist){
               hlist2[irot][itel][iele]=new TH2F(Form("Rot%d_Tel%d_Ele%d",RotateDB::rotindex[irot],RotateDB::telindex[itel],iele+1),";Time;Long Axis [degree]",nxbin,xbins,48,-12.,12.);
               hcount2[irot][itel][iele]=new TH1F(Form("NAve_Rot%d_Tel%d_Ele%d",RotateDB::rotindex[irot],RotateDB::telindex[itel],iele+1),";Time;Events",nxbin,xbins);
            }
         }
      }
   }

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

   pev->DoFit(0,3,true);
   TH1F* hist=pev->GetDistribution(true,0,3,false,false);
   if(!hist) return kTRUE;

   if(process1&&(itype==1&&iangle<nangle1)){
      if(glist1[irot][itel][iangle]){
         double lum,elum;
         lum=hist->IntegralAndError(0,hist->GetNbinsX()+1,elum);
         int np=glist1[irot][itel][iangle]->GetN();
         glist1[irot][itel][iangle]->SetPoint(np,pev->rabbitTime*1.,lum);
         glist1[irot][itel][iangle]->SetPointError(np,0.,elum);
      }
      if(hcount1[irot][itel][iangle]&&hlist1[irot][itel][iangle]){
         int ixbin=hlist1[irot][itel][iangle]->GetXaxis()->FindBin(pev->rabbitTime*1.);
         hcount1[irot][itel][iangle]->Fill(pev->rabbitTime*1.);
         //printf("Fill type=1 rabbitTime=%ld\n",pev->rabbitTime);
         for(int ibin=1;ibin<=hist->GetNbinsX();ibin++){
            hlist1[irot][itel][iangle]->SetBinContent(ixbin,ibin,hlist1[irot][itel][iangle]->GetBinContent(ixbin,ibin)+hist->GetBinContent(ibin));
            hlist1[irot][itel][iangle]->SetBinError(ixbin,ibin,sqrt(pow(hlist1[irot][itel][iangle]->GetBinError(ixbin,ibin),2)+pow(hist->GetBinError(ibin),2)));
         }
      }
   }

   if(process2&&(itype==2&&iangle<nangle2)){
      if(glist2[irot][itel][iangle]){
         double lum,elum;
         lum=hist->IntegralAndError(0,hist->GetNbinsX()+1,elum);
         int np=glist2[irot][itel][iangle]->GetN();
         glist2[irot][itel][iangle]->SetPoint(np,(double)pev->rabbitTime,lum);
         glist2[irot][itel][iangle]->SetPointError(np,0.,elum);
      }
      if(hcount2[irot][itel][iangle]&&hlist2[irot][itel][iangle]){
         int ixbin=hlist2[irot][itel][iangle]->GetXaxis()->FindBin(pev->rabbitTime*1.);
         hcount2[irot][itel][iangle]->Fill(pev->rabbitTime*1.);
         for(int ibin=1;ibin<=hist->GetNbinsX();ibin++){
            hlist2[irot][itel][iangle]->SetBinContent(ixbin,ibin,hlist2[irot][itel][iangle]->GetBinContent(ixbin,ibin)+hist->GetBinContent(ibin));
            hlist2[irot][itel][iangle]->SetBinError(ixbin,ibin,sqrt(pow(hlist2[irot][itel][iangle]->GetBinError(ixbin,ibin),2)+pow(hist->GetBinError(ibin),2)));
         }
      }
   }

   delete hist;

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
      for(int ii=0;ii<20;ii++){
         for(int jj=0;jj<nangle1;jj++){
            for(int irot=0;irot<RotateDB::nrot;irot++){
               if(glist1[irot][ii][jj]) glist1[irot][ii][jj]->Write();
               if(hcount1[irot][ii][jj]) hcount1[irot][ii][jj]->Write();
               if(hlist1[irot][ii][jj]) hlist1[irot][ii][jj]->Write();
            }
         }
         for(int jj=0;jj<nangle2;jj++){
            for(int irot=0;irot<RotateDB::nrot;irot++){
               if(glist2[irot][ii][jj]) glist2[irot][ii][jj]->Write();
               if(hcount2[irot][ii][jj]) hcount2[irot][ii][jj]->Write();
               if(hlist2[irot][ii][jj]) hlist2[irot][ii][jj]->Write();
            }
         }
      }
   }
}
