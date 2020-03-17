#include <iostream>
#include <cstring>
#include "WFTelescope.h"
#include "LHChain.h"
#include "RotateDB.h"
#include "CalibWFCTA.h"
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
   CalibWFCTA::UseSiPMCalibVer=1;
   WFCTAEvent::CalibType=0x7;
   WFCTAEvent::jdebug=0;

   if(argc<5){
      printf("Use %s <runlist> <outname> <iEvent> <iTime> <first> <last> <maxevent> <firstevent> <type> <doclean>\n",argv[0]);
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
   #ifdef _hliu
   char cdir[200]="/afs/ihep.ac.cn/users/h/hliu/Documents/Analysis/LaserEvent";
   #endif
   #ifdef _lix
   char cdir[200]="/scratchfs/ybj/lix/Laser";
   #endif

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
   int type=argc>9?(atoi(argv[9])):12;
   bool doclean=argc>10?(atoi(argv[10])>0):true;

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

   WFCTAEvent::DoDraw=false;
   RotateDB::jdebug=0;
   RotateDB::ntotmin=5;
   RotateDB::nsidemin=2;

   RotateDB* pr=RotateDB::GetHead();

   TFile* fout=0;
   if(strstr(OutType,"root")) fout=TFile::Open(outname,"RECREATE");

   //TH1::SetDefaultSumw2();
   WFCTAEvent* pev=0;
   int nplot=0;
   bool cleanimage=false;
   if(ievent>=0&&itime>1300000000){
      pev=chain.GetEvent(itime,ievent);
      if(pev){
         TCanvas* cc=new TCanvas();
         //pev->DoFit(0,3);
         pev->Draw(type,"colz",cleanimage);
         cc->Print(Form("%s",outname),OutType);
         nplot++;
         //cc=new TCanvas();
         //cc->Print(Form("%s)",outname),OutType);
      }
   }
   else{
      int piEvent=1000000;
      int nfile=0;
      for(int ientry=firstevent;ientry<maxevent;ientry++){
         pev=chain.GetEvent(ientry);
         if(pev->iEvent<piEvent){
            printf("Processing file %d of %d name=%s...\n",nfile+first,last+1,pev->GetFileName());
            printf("iEvent=%d Time=%d+%lf\n",pev->iEvent,pev->rabbitTime,pev->rabbittime*2.0e-8);
            nfile++;
         }
         piEvent=pev->iEvent;

         if(itime>1300000000){if(pev->rabbitTime!=itime) continue;}

         if((ientry%1000)==0) printf("entry=%d of %d iTel=%d event=%d time={%d,%lf}\n",ientry,maxevent,pev->iTel,pev->iEvent,pev->rabbitTime,pev->rabbittime*2.0e-8);
         int index=-1;

         //index=pr->GetEleAzi(pev);
         //if(index<=0) continue;
         //if((index/10)!=1) continue;

         //if(pr->GetLi((double)pev->rabbittime)<0) continue;
         //printf("LaserEvent0: entry=%d event=%d time={%d+%.8lf} index=%d\n",ientry,pev->iEvent,pev->rabbitTime,pev->rabbittime*2.0e-8,index);

         index=pr->LaserIsFine(pev);
         if(doclean) {if(index<=0) continue;}
         if(index!=221) continue;

         printf("LaserEvent: entry=%d event=%d time={%d+%.8lf} index=%d\n",ientry,pev->iEvent,pev->rabbitTime,pev->rabbittime*2.0e-8,index);

         TCanvas* cc=new TCanvas();
         pev->Draw(type,"colz",cleanimage);
         cc->Print(Form("%s%s",outname,nplot==0?"(":""),OutType);
         nplot++;
         //int sipm0=464;
         //printf("Laser: sipm=%d cont={%.2lf,%.2lf}\n",sipm0,pev->GetContent(sipm0,pev->iTel,3,false,true),pev->GetContent(sipm0,pev->iTel,12,false,true));
         //pev->DoFit(0,3);
         //printf("iEvent=%d Time=%d+%.10lf kk={%.2lf+-%.2lf} cc={%.2lf+-%.2lf}\n",pev->iEvent,pev->rabbitTime,pev->rabbittime*20*1.0e-9,pev->minimizer->X()[3]/PI*180,pev->minimizer->Errors()[3]/PI*180,pev->minimizer->X()[2]/PI*180,pev->minimizer->Errors()[2]/PI*180);
         //if(nplot>=1) break;
      }
      TCanvas* cc=new TCanvas();
      cc->Print(Form("%s)",outname),OutType);
   }

   if(pev&&(nplot==30||nplot==1)){
      pev->DoFit(0,3);
      if(pev->minimizer) printf("iEvent=%d Time=%d+%.10lf kk={%.2lf+-%.2lf} cc={%.2lf+-%.2lf}\n",pev->iEvent,pev->rabbitTime,pev->rabbittime*20*1.0e-9,pev->minimizer->X()[3]/PI*180,pev->minimizer->Errors()[3]/PI*180,pev->minimizer->X()[2]/PI*180,pev->minimizer->Errors()[2]/PI*180);
   }

   return 0;
}
