#include <iostream>
#include <cstring>
#include "WFTelescope.h"
#include "LHChain.h"
#include "RotateDB.h"
#include "CalibWFCTA.h"
#include "StatusDB.h"
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
      printf("firstline=%s time=%d\n",firstline,timefirst);
      printf("lastline =%s time=%d\n",lastline,timelast);
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
   int ctype=12;

   WFCTAEvent::npetrigger=50;
   WFCTAEvent::nfiretrigger=3;
   CalibWFCTA::UseSiPMCalibVer=1;
   WFCTAEvent::CalibType=0x7;
   WFCTAEvent::jdebug=0;

   StatusDB::jdebug=0;
   RotateDB::jdebug=0;
   RotateDB::ntotmin=5;
   RotateDB::nsidemin=2;
   RotateDB::phimargin=10;
   RotateDB::ccmargin=1.;

   RotateDB* pr=RotateDB::GetHead();

   // create histogram to save results
   const int nangle1=4;
   TGraphErrors* glist1[10][20][nangle1];
   TH2F* hlist1[10][20][4];
   TH2F* hTeng1[10][20][4];
   TH1F* hcount1[10][20][4];
   for(int irot=0;irot<10;irot++){
      for(int itel=0;itel<20;itel++){
         for(int iazi=0;iazi<nangle1;iazi++){
            glist1[irot][itel][iazi]=0;
            hlist1[irot][itel][iazi]=0;
            hTeng1[irot][itel][iazi]=0;
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
               double Eref=RotateDB::GetHead()->GetEref(RotateDB::rotindex[irot],RotateDB::telindex[itel],1,iazi);
               double Erange[2]={Eref*0.85,Eref*1.15};
               hTeng1[irot][itel][iazi]=new TH2F(Form("Teng_Rot%d_Tel%d_Azi%d",RotateDB::rotindex[irot],RotateDB::telindex[itel],iazi+1),";Time;Total Pe [pe]",nxbin,xbins,100,Erange[0],Erange[1]);
               hTeng1[irot][itel][iazi]->GetXaxis()->SetTimeDisplay(1);
               hcount1[irot][itel][iazi]=new TH1F(Form("NAve_Rot%d_Tel%d_Azi%d",RotateDB::rotindex[irot],RotateDB::telindex[itel],iazi+1),";Time;Events",nxbin,xbins);
               hcount1[irot][itel][iazi]->GetXaxis()->SetTimeDisplay(1);
            }
         }
      }
   }

   const int nangle2=6;
   TGraphErrors* glist2[10][20][nangle2];
   TH2F* hlist2[10][20][nangle2];
   TH2F* hTeng2[10][20][nangle2];
   TH1F* hcount2[10][20][nangle2];
   for(int irot=0;irot<10;irot++){
      for(int itel=0;itel<20;itel++){
         for(int iele=0;iele<nangle2;iele++){
            glist2[irot][itel][iele]=0;
            hlist2[irot][itel][iele]=0;
            hTeng2[irot][itel][iele]=0;
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
               hlist2[irot][itel][iele]->GetXaxis()->SetTimeDisplay(1);
               double Eref=RotateDB::GetHead()->GetEref(RotateDB::rotindex[irot],RotateDB::telindex[itel],2,iele);
               double Erange[2]={Eref*0.85,Eref*1.15};
               hTeng2[irot][itel][iele]=new TH2F(Form("Teng_Rot%d_Tel%d_Ele%d",RotateDB::rotindex[irot],RotateDB::telindex[itel],iele+1),";Time;Total Pe [pe]",nxbin,xbins,100,Erange[0],Erange[1]);
               hTeng2[irot][itel][iele]->GetXaxis()->SetTimeDisplay(1);
               hcount2[irot][itel][iele]=new TH1F(Form("NAve_Rot%d_Tel%d_Ele%d",RotateDB::rotindex[irot],RotateDB::telindex[itel],iele+1),";Time;Events",nxbin,xbins);
               hcount2[irot][itel][iele]->GetXaxis()->SetTimeDisplay(1);
            }
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

   TH1::SetDefaultSumw2();
   int piEvent=1000;
   int nfile=0;
   for(int ientry=firstevent;ientry<maxevent;ientry++){
      WFCTAEvent* pev=chain.GetEvent(ientry);
      if(pev->iEvent<piEvent){
         printf("Processing file %d of %d name=%s...\n",nfile+first,last+1,pev->GetFileName());
         nfile++;
      }
      //char filename[200]="";
      //bool exist=CommonTools::GetStatusFile(filename,(char*)pev->GetFileName());
      //cout<<"Time="<<pev->rabbitTime<<", Temp="<<StatusDB::GetPreTemp(pev->iTel,pev->rabbitTime,5,exist?filename:0)<<endl;

      piEvent=pev->iEvent;
      int irot=pr->GetLi(double(pev->rabbittime));
      int itel=pr->GetTi(pev->iTel);

      if((ientry%1000)==0) printf("entry=%d of %d iTel=%d event=%d time={%d,%lf}\n",ientry,maxevent,pev->iTel,pev->iEvent,pev->rabbitTime,pev->rabbittime);
      if(irot<0) continue;
      //printf("LaserEvent0: entry=%d time=%d event=%d irot=%d\n",ientry,pev->rabbitTime,pev->iEvent,irot);
      int index=-1;

      //index=pr->GetEleAzi(pev);
      //if(index<=0) continue;

      index=pr->LaserIsFine(pev);
      //printf("LaserEvent1: entry=%d time=%d event=%d  index=%d\n",ientry,pev->rabbitTime,pev->iEvent,index);
      if(index<=0) continue;

      int itype=((index%100)/10);
      int iangle=(index%10);

      printf("LaserEvent2: entry=%d time=%d event=%d  index=%d\n",ientry,pev->rabbitTime,pev->iEvent,index);

      pev->DoFit(0,3);
      TH1F* hist=pev->GetDistribution(true,0,ctype,false,false);
      if(!hist) continue;

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
            for(int ibin=1;ibin<=hist->GetNbinsX();ibin++){
               hlist1[irot][itel][iangle]->SetBinContent(ixbin,ibin,hlist1[irot][itel][iangle]->GetBinContent(ixbin,ibin)+hist->GetBinContent(ibin));
               hlist1[irot][itel][iangle]->SetBinError(ixbin,ibin,sqrt(pow(hlist1[irot][itel][iangle]->GetBinError(ixbin,ibin),2)+pow(hist->GetBinError(ibin),2)));
            }
         }
         if(hTeng1[irot][itel][iangle]){
            int nsipm;
            double error;
            hTeng1[irot][itel][iangle]->Fill(pev->rabbitTime*1.,pev->GetTotalPe(error,nsipm,pev->iTel,ctype));
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
         if(hTeng2[irot][itel][iangle]){
            int nsipm;
            double error;
            hTeng2[irot][itel][iangle]->Fill(pev->rabbitTime*1.,pev->GetTotalPe(error,nsipm,pev->iTel,ctype));
         }
      }

      //delete hist;
   }

   fout->cd();
   for(int ii=0;ii<20;ii++){
      for(int jj=0;jj<nangle1;jj++){
         for(int irot=0;irot<RotateDB::nrot;irot++){
            if(glist1[irot][ii][jj]) glist1[irot][ii][jj]->Write();
            if(hcount1[irot][ii][jj]) hcount1[irot][ii][jj]->Write();
            if(hlist1[irot][ii][jj]) hlist1[irot][ii][jj]->Write();
            if(hTeng1[irot][ii][jj]) hTeng1[irot][ii][jj]->Write();
         }
      }
      for(int jj=0;jj<nangle2;jj++){
         for(int irot=0;irot<RotateDB::nrot;irot++){
            if(glist2[irot][ii][jj]) glist2[irot][ii][jj]->Write();
            if(hcount2[irot][ii][jj]) hcount2[irot][ii][jj]->Write();
            if(hlist2[irot][ii][jj]) hlist2[irot][ii][jj]->Write();
            if(hTeng2[irot][ii][jj]) hTeng2[irot][ii][jj]->Write();
         }
      }
   }
   fout->Close();
   return 0;
}
