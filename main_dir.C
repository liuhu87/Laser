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

   // create histogram to save results
   TH2F* hkk1[ntel][4];
   TH2F* hcc1[ntel][4];
   for(int itel=0;itel<ntel;itel++){
      for(int iazi=0;iazi<4;iazi++){
         hkk1[itel][iazi]=0;
         hcc1[itel][iazi]=0;
         if(!process1) continue;
         if(iazi==0&&(itel!=4&&itel!=5)) continue;
         if(iazi==1&&(itel!=3&&itel!=4)) continue;
         if(iazi==2&&(itel!=2&&itel!=3)) continue;
         if(iazi==3&&(itel!=0&&itel!=1&&itel!=2)) continue;
         hkk1[itel][iazi]=new TH2F(Form("Tel%d_Azi%d_kk",itel+1,iazi+1),";Time;kk [degree]",nxbin,xbins,360,0,180);
         hcc1[itel][iazi]=new TH2F(Form("Tel%d_Azi%d_cc",itel+1,iazi+1),";Time;cc [degree]",nxbin,xbins,48,-12,12);
      }
   }

   const int nrot=2;
   int rotindex[nrot]={2,3};
   double rottime[nrot]={990016000,990845000};
   TH2F* hkk2[nrot][ntel][6];
   TH2F* hcc2[nrot][ntel][6];
   for(int irot=0;irot<nrot;irot++){
      for(int itel=0;itel<ntel;itel++){
         for(int iele=0;iele<6;iele++){
            hkk2[irot][itel][iele]=0;
            hcc2[irot][itel][iele]=0;
            if(!process2) continue;
            hkk2[irot][itel][iele]=new TH2F(Form("Rot%d_Tel%d_Ele%d_kk",rotindex[irot],itel+1,iele+1),";Time;kk [degree]",nxbin,xbins,360,0,180);
            hcc2[irot][itel][iele]=new TH2F(Form("Rot%d_Tel%d_Ele%d_cc",rotindex[irot],itel+1,iele+1),";Time;cc [degree]",nxbin,xbins,48,-12,12);
         }
      }
   }

   RotateDB::jdebug=0;
   RotateDB::ntotmin=5;
   RotateDB::nsidemin=2;

   RotateDB* pr=RotateDB::GetHead();

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
   for(int ientry=firstevent;ientry<maxevent;ientry++){
      WFCTAEvent* pev=chain.GetEvent(ientry);

      if((ientry%1000)==0) printf("entry=%d of %d iTel=%d event=%d time={%d,%lf}\n",ientry,maxevent,pev->iTel,pev->iEvent,pev->rabbitTime,pev->rabbittime);

      //if(!pev->PassClean(pev->iTel,3)) continue;
      //if(pev->IsLed(900)) continue;
      //if(fabs(pev->rabbittime*20-990022580)>16000) continue;
      //if(fabs(pev->rabbittime*20-990016000)>1600*10) continue;
      //if(abs(pev->rabbittime*20-990016000)>1600*20) continue; //L2
      //if(abs(pev->rabbittime*20-990845000)>1600*20) continue; //L3
      int Li=-1;
      for(int irot=0;irot<nrot;irot++){
         if(abs(pev->rabbittime*20-rottime[irot])<1600*20) {Li=rotindex[irot]; break;}
      }
      if(Li<0) continue;

      printf("LaserEvent: entry=%d time=%d\n",ientry,pev->rabbitTime);

      int DoLoad=pr->GetEleAzi(pev->rabbitTime,Li,pev->iTel);
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
