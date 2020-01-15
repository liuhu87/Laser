#include <iostream>
#include <cstring>
#include "WFTelescope.h"
#include "LHChain.h"
#include "RotateDB.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TStyle.h"
#include "TPaletteAxis.h"
#include "TColor.h"
#include "TMath.h"

using namespace std;
const int timeshift=788918400;
int main(int argc,char* argv[]){
   gStyle->SetNumberContours(50);
   gStyle->SetOptStat(0);
   gStyle->SetOptLogy(0);
   gStyle->SetOptLogz(0);
   gStyle->SetPadGridX(1);
   gStyle->SetPadGridY(1);
   gStyle->SetPalette(1);
   WFCTAEvent::DoDraw=false;

   if(argc<5){
      printf("Use %s <runlist> <outname> <iEvent> <iTime> <first> <last> <maxevent> <firstevent>\n",argv[0]);
      return 0;
   }

   char* runlist=argv[1];
   char* outname=argv[2];
   char filename[100];
   char OutType[20];
   int length=strlen(outname);
   int iloc=-1;
   for(int ii=length-1;ii>=0;ii--){
      if(outname[ii]=='.'){ iloc=ii; break;}
   }
   for(int ii=0;ii<length;ii++){
      if(ii<iloc) filename[ii]=outname[ii];
      else if(ii==iloc) filename[ii]='\0';
      else OutType[ii-iloc-1]=outname[ii];
   }
   OutType[length-1-iloc]='\0';
   //printf("Outname=%s (%s.%s)\n",outname,filename,OutType);
   char cdir[200]="/afs/ihep.ac.cn/users/h/hliu/Documents/Analysis/LaserEvent";

   char firstline[300];
   char lastline[300];
   int nline=CommonTools::GetFirstLastLine(Form("%s/%s",cdir,runlist),firstline,lastline);
   int timefirst=CommonTools::GetTimeFromFileName(firstline)-600;
   int timelast=CommonTools::GetTimeFromFileName(lastline)+600;
   //timefirst=1573061985;
   //timelast=1573062011;
   printf("firstline=%s\n",firstline);
   printf("lastline =%s\n",lastline);
   printf("nline=%d timefirst=%d timelast=%d\n",nline,timefirst,timelast);
   double xbins[100000];
   int nxbin=CommonTools::GetBins(timefirst,timelast,600.,xbins);
   printf("Total %d time bins\n",nxbin);
   if(nxbin==100000-1){
      printf("BinError,Please Increase BinWidth, Exiting...\n");
      return 0;
   }

   int ievent=atoi(argv[3]);
   int itime=atoi(argv[4]);
   int first=argc>5?atoi(argv[5]):0;
   int last=argc>6?atoi(argv[6]):(nline-1);
   int maxevent=argc>7?atoi(argv[7]):-1;
   int firstevent=argc>8?atoi(argv[8]):0;

   const double pi=3.1415926;
   LHChain chain;
   if(strstr(runlist,".root")){
      chain.Add(runlist);
   }
   else{
      chain.AddFromFile(Form("%s/%s",cdir,runlist),first,last);
   }
   if(maxevent<=0) maxevent=chain.GetEntries();
   maxevent=TMath::Min(maxevent,(int)chain.GetEntries());

   RotateDB::jdebug=0;
   RotateDB::ntotmin=5;
   RotateDB::nsidemin=2;

   RotateDB* pr=RotateDB::GetHead();

   TFile* fout=0;
   if(strstr(OutType,"root")) fout=TFile::Open(outname,"RECREATE");

   //TH1::SetDefaultSumw2();
   int nplot=0;
   if(ievent>=0&&itime>1300000000){
      WFCTAEvent* pev=chain.GetEvent(itime,ievent);
      if(pev){
         TCanvas* cc=new TCanvas();
         pev->Draw(3,"colz",false);
         cc->Print(Form("%s",outname),OutType);
         nplot++;
         //cc=new TCanvas();
         //cc->Print(Form("%s)",outname),OutType);
      }
   }
   else{
      for(int ientry=firstevent;ientry<maxevent;ientry++){
         WFCTAEvent* pev=chain.GetEvent(ientry);
         //if(pev->iEvent!=ievent||pev->rabbitTime!=itime) continue;

         if((ientry%1000)==0) printf("entry=%d of %d iTel=%d event=%d time={%d,%lf}\n",ientry,maxevent,pev->iTel,pev->iEvent,pev->rabbitTime,pev->rabbittime);

         int Li=pr->GetLi(pev->rabbittime);
         if(Li<0) continue;

         printf("LaserEvent: entry=%d time=%d\n",ientry,pev->rabbitTime);

         int DoLoad=pr->GetEleAzi(pev->rabbitTime,Li,pev->iTel);
         if(DoLoad<0) continue;

         pev->DoFit(0,3);
         if(!pr->IsFineImage(pev,Li,DoLoad)) continue;

         TCanvas* cc=new TCanvas();
         pev->Draw(3,"colz",false);
         cc->Print(Form("%s%s",outname,nplot==0?"(":""),OutType);
         nplot++;
         //pev->DoFit(0,3);
         //printf("iEvent=%d Time=%d+%.10lf kk={%.2lf+-%.2lf} cc={%.2lf+-%.2lf}\n",pev->iEvent,pev->rabbitTime,pev->rabbittime*20*1.0e-9,pev->minimizer->X()[3]/PI*180,pev->minimizer->Errors()[3]/PI*180,pev->minimizer->X()[2]/PI*180,pev->minimizer->Errors()[2]/PI*180);
      }
      TCanvas* cc=new TCanvas();
      cc->Print(Form("%s)",outname),OutType);
   }

   return 0;
}