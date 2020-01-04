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

   if(argc<4){
      printf("Use %s <runlist> <outname> <first> <last> <maxevent> <firstevent>\n",argv[0]);
      return 0;
   }

   char* runlist=argv[1];
   char* outname=argv[2];

   char firstline[300];
   char lastline[300];
   int nline=CommonTools::GetFirstLastLine(Form("/afs/ihep.ac.cn/users/h/hliu/Documents/Analysis/LaserEvent/%s",runlist),firstline,lastline);
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

   int first=argc>3?atoi(argv[3]):0;
   int last=argc>4?atoi(argv[4]):(nline-1);
   int maxevent=argc>5?atoi(argv[5]):-1;
   int firstevent=argc>6?atoi(argv[6]):0;
   const int ntel=6;
   bool process1=true;
   bool process2=true;

   WFTelescopeArray::jdebug=0;
   WFTelescopeArray::DoSim=true;
   WFTelescopeArray::GetHead(Form("/afs/ihep.ac.cn/users/h/hliu/Documents/LHAASO/WFCTA/default.inp"));
   WFCTAEvent::npetrigger=50;
   WFCTAEvent::nfiretrigger=3;

   RotateDB::jdebug=0;
   RotateDB::ntotmin=5;
   RotateDB::nsidemin=2;

   RotateDB* pr=RotateDB::GetHead();

   double AngleList1[4][2];
   AngleList1[0][0]=40; AngleList1[0][1]=42;
   AngleList1[1][0]=40; AngleList1[1][1]=29;
   AngleList1[2][0]=40; AngleList1[2][1]=19;
   AngleList1[3][0]=40; AngleList1[3][1]=4;

   //T1-6
   double elelist[6]={10,20,30,40,50,55};
   double azilist[ntel][6]={{23.7,19,15,10,1,-5},
                         {23.7,17,11,0,-12,-18},
                         {24.5,22,19,15,10,3},
                         {27,26,26,26,25,24},
                         {30.5,33,35,38,42,44},
                         {32,38,43,47,54,61}
                        };
   double AngleList2[ntel][6][2];
   for(int itel=0;itel<ntel;itel++){
      for(int iele=0;iele<6;iele++){
         AngleList2[itel][iele][0]=elelist[iele];
         AngleList2[itel][iele][1]=azilist[itel][iele];
      }
   }

   //histogram
   TH2F* hlist1[ntel][4];
   TH1F* hcount1[ntel][4];
   for(int itel=0;itel<ntel;itel++){
      for(int iazi=0;iazi<4;iazi++){
         hlist1[itel][iazi]=0;
         hcount1[itel][iazi]=0;
         if(!process1) continue;
         if(iazi==0&&(itel!=4&&itel!=5)) continue;
         if(iazi==1&&(itel!=3&&itel!=4)) continue;
         if(iazi==2&&(itel!=2&&itel!=3)) continue;
         if(iazi==3&&(itel!=0&&itel!=1&&itel!=2)) continue;
         hlist1[itel][iazi]=new TH2F(Form("Tel%d_Azi%d",itel+1,iazi+1),";Time;Long Axis [degree]",nxbin,xbins,48,-12.,12.);
         hcount1[itel][iazi]=new TH1F(Form("NAve_Tel%d_Azi%d",itel+1,iazi+1),";Time;Events",nxbin,xbins);
      }
   }

   TH2F* hlist2[ntel][6];
   TH1F* hcount2[ntel][6];
   for(int itel=0;itel<ntel;itel++){
      for(int iele=0;iele<6;iele++){
         hlist2[itel][iele]=0;
         hcount2[itel][iele]=0;
         if(!process2) continue;
         hlist2[itel][iele]=new TH2F(Form("Tel%d_Ele%d",itel+1,iele+1),";Time;Long Axis [degree]",nxbin,xbins,48,-12.,12.);
         hcount2[itel][iele]=new TH1F(Form("NAve_Tel%d_Ele%d",itel+1,iele+1),";Time;Events",nxbin,xbins);
      }
   }

   const double pi=3.1415926;
   LHChain chain;
   chain.AddFromFile(Form("/afs/ihep.ac.cn/users/h/hliu/Documents/Analysis/LaserEvent/%s",runlist),first,last);
   if(maxevent<=0) maxevent=chain.GetEntries();
   maxevent=TMath::Min(maxevent,(int)chain.GetEntries());

   TFile* fout=TFile::Open(outname,"RECREATE");

   //TH1::SetDefaultSumw2();
   for(int ientry=firstevent;ientry<maxevent;ientry++){
      WFCTAEvent* pev=chain.GetEvent(ientry);

      if((ientry%1000)==0) printf("entry=%d of %d iTel=%d event=%d time={%d,%lf}\n",ientry,maxevent,pev->iTel,pev->iEvent,pev->rabbitTime,pev->rabbittime);

      //if(!pev->PassClean(pev->iTel,3)) continue;
      //if(pev->IsLed(900)) continue;
      //if(fabs(pev->rabbittime*20-990022580)>16000) continue;
      if(fabs(pev->rabbittime*20-990016000)>1600*10) continue;

      printf("LaserEvent: entry=%d time=%d\n",ientry,pev->rabbitTime);

      int DoLoad=pr->GetEleAzi(pev->rabbitTime,2,pev->iTel);
      if(DoLoad<0) continue;

      printf("Rotate Log: time=%d ele=%lf azi=%lf\n",pev->rabbitTime,pr->GetElevation(),pr->GetAzimuth());

      bool laseropen=pr->GetLaserSwith();
      if(!laseropen) continue;
      double height=pr->GetHeight();
      if(height<200) continue;

      double elevation=pr->GetElevation();
      double azimuth=pr->GetAzimuth();

      int azii1=-1,teli1=-1;
      if(process1&&(DoLoad/10)==1){
         teli1=pev->iTel-1;
         azii1=(DoLoad%10);
      }
      int elei2=-1,teli2=-1;
      if(process2&&(DoLoad/10)==2){
         teli2=pev->iTel-1;
         elei2=(DoLoad%10);
      }

      pev->DoFit(0,3);
      TH1F* hist=pev->GetDistribution(true,0,3,false,false);
      if(!hist) continue;
      //printf("hist=%p\n",hist);

      //TCanvas* cc=new TCanvas(Form("entry%d",ientry),"");
      //TH2Poly* h2=pev->Draw(3,"colz",false);
      //h2->Draw("colz");
      //cc->Print(Form("event%d.pdf",ientry),"pdf");

      if(process1&&(azii1>=0&&teli1>=0)){
         int ixbin=hlist1[teli1][azii1]->GetXaxis()->FindBin(pev->rabbitTime*1.);
         hcount1[teli1][azii1]->Fill(pev->rabbitTime*1.);
         for(int ibin=1;ibin<=hist->GetNbinsX();ibin++){
            hlist1[teli1][azii1]->SetBinContent(ixbin,ibin,hlist1[teli1][azii1]->GetBinContent(ixbin,ibin)+hist->GetBinContent(ibin));
            hlist1[teli1][azii1]->SetBinError(ixbin,ibin,sqrt(pow(hlist1[teli1][azii1]->GetBinError(ixbin,ibin),2)+pow(hist->GetBinError(ibin),2)));
         }
      }
      if(process2&&(elei2>=0&&teli2>=0)){
         int ixbin=hlist2[teli2][elei2]->GetXaxis()->FindBin(pev->rabbitTime*1.);
         hcount2[teli2][elei2]->Fill(pev->rabbitTime*1.);
         for(int ibin=1;ibin<=hist->GetNbinsX();ibin++){
            hlist2[teli2][elei2]->SetBinContent(ixbin,ibin,hlist2[teli2][elei2]->GetBinContent(ixbin,ibin)+hist->GetBinContent(ibin));
            hlist2[teli2][elei2]->SetBinError(ixbin,ibin,sqrt(pow(hlist2[teli2][elei2]->GetBinError(ixbin,ibin),2)+pow(hist->GetBinError(ibin),2)));
         }
      }

      delete hist;
   }

   fout->cd();
   for(int ii=0;ii<ntel;ii++){
      for(int jj=0;jj<4;jj++){
         if(hcount1[ii][jj]) hcount1[ii][jj]->Write();
         if(hlist1[ii][jj]) hlist1[ii][jj]->Write();
      }
      for(int jj=0;jj<6;jj++){
         if(hcount2[ii][jj]) hcount2[ii][jj]->Write();
         if(hlist2[ii][jj]) hlist2[ii][jj]->Write();
      }
   }
   fout->Close();
   return 0;
}
