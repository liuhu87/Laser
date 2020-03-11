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
#include "TGraphErrors.h"

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
   gStyle->SetMarkerStyle(20);
   gStyle->SetMarkerSize(1.0);
   gStyle->SetMarkerColor(4);
   gStyle->SetLineWidth(2);
   gStyle->SetLineColor(4);
   gStyle->SetHistLineWidth(2);
   gStyle->SetHistLineColor(4);
   WFCTAEvent::DoDraw=false;

   if(argc<4){
      printf("Use %s <runlist> <outname> <first> <last> <maxevent> <firstevent>\n",argv[0]);
      return 0;
   }

   char* runlist=argv[1];
   char* outname=argv[2];
   #ifdef _hliu
   char cdir[200]="/afs/ihep.ac.cn/users/h/hliu/Documents/Analysis/LaserEvent";
   #endif
   #ifdef _lix
   char cdir[200]="/scratchfs/ybj/lix/Laser";
   #endif
   int first,last;
   int maxevent=argc>5?atoi(argv[5]):-1;
   int firstevent=argc>6?atoi(argv[6]):0;

   int timefirst,timelast,nline;
   if(strstr(runlist,".root")){
      nline=1;
      timefirst=CommonTools::GetTimeFromFileName(runlist)-600;
      timelast=CommonTools::GetTimeFromFileName(runlist)+3600+600;
      first=0;
      last=0;
   }
   else{
      char firstline[300];
      char lastline[300];
      nline=CommonTools::GetFirstLastLine(Form("%s/%s",cdir,runlist),firstline,lastline);
      timefirst=CommonTools::GetTimeFromFileName(firstline)-600;
      timelast=CommonTools::GetTimeFromFileName(lastline)+600;
      printf("firstline=%s\n",firstline);
      printf("lastline =%s\n",lastline);
      first=argc>3?atoi(argv[3]):0;
      last=argc>4?atoi(argv[4]):(nline-1);
   }
   printf("nline=%d timefirst=%d timelast=%d\n",nline,timefirst,timelast);
   double xbins[100000];
   int nxbin=CommonTools::GetBins(timefirst,timelast,600.,xbins);
   printf("Total %d time bins\n",nxbin);
   if(nxbin==100000-1){
      printf("BinError,Please Increase BinWidth, Exiting...\n");
      return 0;
   }

   bool process1=true;
   bool process2=true;
   bool UseGraph=true;
   bool UseHist=true;

   WFCTAEvent::npetrigger=50;
   WFCTAEvent::nfiretrigger=3;

   RotateDB::jdebug=0;
   RotateDB::ntotmin=5;
   RotateDB::nsidemin=2;
   RotateDB::phimargin=25;
   RotateDB::ccmargin=2.5;

   RotateDB* pr=RotateDB::GetHead();

   // create histogram to save results
   const int nangle1=4;
   TGraphErrors* gphi1[20][nangle1];
   TGraphErrors* gcc1[20][nangle1];
   TH2F* hphi1[20][nangle1];
   TH2F* hcc1[20][nangle1];
   for(int itel=0;itel<20;itel++){
      for(int iazi=0;iazi<nangle1;iazi++){
         hphi1[itel][iazi]=0;
         hcc1[itel][iazi]=0;
         gphi1[itel][iazi]=0;
         gcc1[itel][iazi]=0;
         if(!process1) continue;
         if(iazi==0&&(RotateDB::telindex[itel]!=5&&RotateDB::telindex[itel]!=6)) continue;
         if(iazi==1&&(RotateDB::telindex[itel]!=4&&RotateDB::telindex[itel]!=5)) continue;
         if(iazi==2&&(RotateDB::telindex[itel]!=3&&RotateDB::telindex[itel]!=4)) continue;
         if(iazi==3&&(RotateDB::telindex[itel]!=1&&RotateDB::telindex[itel]!=2&&RotateDB::telindex[itel]!=3)) continue;
         if(UseGraph){
            gphi1[itel][iazi]=new TGraphErrors();
            gphi1[itel][iazi]->SetName(Form("gTel%d_Azi%d_phi",RotateDB::telindex[itel],iazi+1));
            gphi1[itel][iazi]->GetXaxis()->SetTitle("Time");
            gphi1[itel][iazi]->GetXaxis()->SetTimeDisplay(1);
            gphi1[itel][iazi]->GetXaxis()->SetNdivisions(-203);
            gphi1[itel][iazi]->GetXaxis()->SetTimeFormat("%Mm/%Hh/%d/%m/%Y%F1970-01-01 00:00:00s0");
            gphi1[itel][iazi]->GetYaxis()->SetTitle("delta_phi [degree]");
            gcc1[itel][iazi]=new TGraphErrors();
            gcc1[itel][iazi]->SetName(Form("gTel%d_Azi%d_cc",RotateDB::telindex[itel],iazi+1));
            gcc1[itel][iazi]->GetXaxis()->SetTitle("Time");
            gcc1[itel][iazi]->GetXaxis()->SetTimeDisplay(1);
            gcc1[itel][iazi]->GetXaxis()->SetNdivisions(-203);
            gcc1[itel][iazi]->GetXaxis()->SetTimeFormat("%Mm/%Hh/%d/%m/%Y%F1970-01-01 00:00:00s0");
            gcc1[itel][iazi]->GetYaxis()->SetTitle("delta_cc [degree]");
         }
         if(UseHist){
            hphi1[itel][iazi]=new TH2F(Form("Tel%d_Azi%d_phi",RotateDB::telindex[itel],iazi+1),";Time;delta_phi [degree]",nxbin,xbins,40,-5,5);
            hphi1[itel][iazi]->GetXaxis()->SetTimeDisplay(1);
            hcc1[itel][iazi]=new TH2F(Form("Tel%d_Azi%d_cc",RotateDB::telindex[itel],iazi+1),";Time;delta_cc [degree]",nxbin,xbins,40,-0.5,0.5);
            hcc1[itel][iazi]->GetXaxis()->SetTimeDisplay(1);
         }
      }
   }

   const int nangle2=6;
   TH1F* hphi2[10][20][nangle2];
   TH1F* hcc2[10][20][nangle2];
   for(int irot=0;irot<10;irot++){
      for(int itel=0;itel<20;itel++){
         for(int iele=0;iele<nangle2;iele++){
            hphi2[irot][itel][iele]=0;
            hcc2[irot][itel][iele]=0;
            if(!process2) continue;
            if(irot>=RotateDB::nrot) continue;
            if(itel>=RotateDB::ntel) continue;
            hphi2[irot][itel][iele]=new TH1F(Form("Rot%d_Tel%d_Ele%d_phi",RotateDB::rotindex[irot],RotateDB::telindex[itel],iele+1),";delta phi [degree]",100,-5,5);
            hcc2[irot][itel][iele]=new TH1F(Form("Rot%d_Tel%d_Ele%d_cc",RotateDB::rotindex[irot],RotateDB::telindex[itel],iele+1),";delta cc [degree]",100,-0.5,0.5);
         }
      }
   }

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

   TFile* fout=TFile::Open(outname,"RECREATE");

   //TH1::SetDefaultSumw2();
   int piEvent=1000;
   int nfile=0;
   for(int ientry=firstevent;ientry<maxevent;ientry++){
      WFCTAEvent* pev=chain.GetEvent(ientry);
      if(pev->iEvent<piEvent){
         printf("Processing file %d of %d ...\n",nfile+first,last+1);
         nfile++;
      }
      piEvent=pev->iEvent;
      int irot=pr->GetLi(double(pev->rabbittime));
      int itel=pr->GetTi(pev->iTel);

      if((ientry%1000)==0) printf("entry=%d of %d iTel=%d event=%d time={%d,%lf}\n",ientry,maxevent,pev->iTel,pev->iEvent,pev->rabbitTime,pev->rabbittime);
      int index=-1;

      //index=pr->GetEleAzi(pev);
      //if(index<=0) continue;

      index=pr->LaserIsFine(pev);
      if(index<=0) continue;

      int itype=(index/10);
      int iangle=(index%10);

      printf("LaserEvent: entry=%d time=%d index=%d\n",ientry,pev->rabbitTime,index);

      double minphi,mincc;
      RotateDB::GetMinDistFit(pev,index,RotateDB::rotindex[irot],minphi,mincc);
      //printf("iTel=%d index=%d Li=%d minphi=%.2lf mincc=%.2lf\n",RotateDB::telindex[itel],index,RotateDB::rotindex[irot],minphi,mincc);

      if(process1&&(itype==1&&iangle<nangle1)){
         if(gphi1[itel][iangle]) gphi1[itel][iangle]->SetPoint(gphi1[itel][iangle]->GetN(),pev->rabbitTime,minphi);
         if(gcc1[itel][iangle]) gcc1[itel][iangle]->SetPoint(gcc1[itel][iangle]->GetN(),pev->rabbitTime,mincc);
         if(hphi1[itel][iangle]) hphi1[itel][iangle]->Fill(pev->rabbitTime*1.,minphi);
         if(hcc1[itel][iangle]) hcc1[itel][iangle]->Fill(pev->rabbitTime*1.,mincc);
      }
      if(process2&&(itype==2&&iangle<nangle2)){
         if(hphi2[irot][itel][iangle]) hphi2[irot][itel][iangle]->Fill(minphi);
         if(hcc2[irot][itel][iangle]) hcc2[irot][itel][iangle]->Fill(mincc);
      }
   }

   fout->cd();
   for(int ii=0;ii<20;ii++){
      for(int jj=0;jj<nangle1;jj++){
         if(gphi1[ii][jj]) gphi1[ii][jj]->Write();
         if(gcc1[ii][jj]) gcc1[ii][jj]->Write();
         if(hphi1[ii][jj]) hphi1[ii][jj]->Write();
         if(hcc1[ii][jj]) hcc1[ii][jj]->Write();
      }
      for(int jj=0;jj<nangle2;jj++){
         for(int irot=0;irot<RotateDB::nrot;irot++){
         if(hphi2[irot][ii][jj]) hphi2[irot][ii][jj]->Write();
         if(hcc2[irot][ii][jj]) hcc2[irot][ii][jj]->Write();
         }
      }
   }
   fout->Close();
   return 0;
}
