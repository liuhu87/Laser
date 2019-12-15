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

   WFTelescopeArray::jdebug=0;
   WFTelescopeArray::DoSim=true;
   WFTelescopeArray::GetHead(Form("/afs/ihep.ac.cn/users/h/hliu/Documents/LHAASO/WFCTA/default.inp"));
   WFCTAEvent::npetrigger=50;
   WFCTAEvent::nfiretrigger=3;

   RotateDB* pr=RotateDB::GetHead();
   //double AngleList1[4][2];
   //AngleList[0][0]=40; AngleList[0][1]=42;
   //AngleList[1][0]=40; AngleList[1][1]=29;
   //AngleList[2][0]=40; AngleList[2][1]=19;
   //AngleList[3][0]=40; AngleList[3][1]=4;

   //T1-6
   double elelist[6]={10,20,30,40,50,55};
   double azilist[6][6]={{23.7,19,15,10,1,-5},
                         {23.7,17,11,0,-12,-18},
                         {24.5,22,19,15,10,3},
                         {27,26,26,26,25,24},
                         {30.5,33,35,38,42,44},
                         {32,38,43,47,54,61}
                        };
   double AngleList2[6][6][2];
   for(int itel=0;itel<6;itel++){
      for(int iele=0;iele<6;iele++){
         AngleList2[itel][iele][0]=elelist[iele];
         AngleList2[itel][iele][1]=azilist[itel][iele];
      }
   }

   //histogram
   TH2F* hlist2[6][6];
   TH1F* hcount2[6][6];
   for(int itel=0;itel<6;itel++){
      for(int iele=0;iele<6;iele++){
         hlist2[itel][iele]=0;
         hcount2[itel][iele]=0;
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

      if((ientry%100)==0) printf("entry=%d of %d iTel=%d event=%d time={%d,%lf}\n",ientry,maxevent,pev->iTel,pev->iEvent,pev->rabbitTime,pev->rabbittime);

      //if(!pev->PassClean(pev->iTel,3)) continue;
      //if(pev->IsLed(900)) continue;
      if(fabs(pev->rabbittime*20-990022580)>16000) continue;

      //printf("LaserEvent: entry=%d time=%d\n",ientry,pev->rabbitTime);

      bool DoLoad=pr->GetEleAzi(pev->rabbitTime,2);
      if(!DoLoad) continue;

      printf("Rotate Log: time=%d ele=%lf azi=%lf\n",pev->rabbitTime,pr->varinfo[9],pr->varinfo[10]);

      double elevation=pr->varinfo[9];
      double azimuth=pr->varinfo[10];

      int elei=-1,teli=-1;
      for(int ii=0;ii<6;ii++){
         for(int jj=0;jj<6;jj++){
            if(fabs(elevation-AngleList2[ii][jj][0])>0.02) continue;
            if(fabs(azimuth-AngleList2[ii][jj][1])>0.02) continue;
            elei=jj;
            teli=ii;
            break;
         }
      }
      if((teli+1)!=pev->iTel) continue;
      //printf("itel=%d iele=%d\n",teli,elei);

      pev->DoFit(0,3);
      TH1F* hist=pev->GetDistribution(true,0,11,false,false);
      if(!hist) continue;
      //printf("hist=%p\n",hist);

      //TCanvas* cc=new TCanvas(Form("entry%d",ientry),"");
      //TH2Poly* h2=pev->Draw(3,"colz",false);
      //h2->Draw("colz");
      //cc->Print(Form("event%d.pdf",ientry),"pdf");

      int ixbin=hlist2[teli][elei]->GetXaxis()->FindBin(pev->rabbitTime*1.);
      hcount2[teli][elei]->Fill(pev->rabbitTime*1.);
      for(int ibin=1;ibin<=hist->GetNbinsX();ibin++){
         hlist2[teli][elei]->SetBinContent(ixbin,ibin,hlist2[teli][elei]->GetBinContent(ixbin,ibin)+hist->GetBinContent(ibin));
         hlist2[teli][elei]->SetBinError(ixbin,ibin,sqrt(pow(hlist2[teli][elei]->GetBinError(ixbin,ibin),2)+pow(hist->GetBinError(ibin),2)));
      }
      delete hist;
   }

   fout->cd();
   for(int ii=0;ii<6;ii++){
      for(int jj=0;jj<6;jj++){
         hcount2[ii][jj]->Write();
         hlist2[ii][jj]->Write();
      }
   }
   fout->Close();
   return 0;
}
