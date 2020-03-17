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
#include "DST.h"

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

   //some external correction
   //int Nentry;
   //double NTime[Tmaxentry];
   //double NAdcH[Tmaxentry][1024];
   //double NBaseH[Tmaxentry][1024];
   //double NTemp_SiPM[Tmaxentry][1024];
   //double NTemp_SiPM_Factor[Tmaxentry][1024];
   //double NTemp_LEDDR_Factor[Tmaxentry][1024];
   //double Ntable_Factor[Tmaxentry][1024];
   //double NDoor1[Tmaxentry];
   //double NDoor2[Tmaxentry];
   //double NTemp_LEDDR[Tmaxentry];

   TDirectory* gdir=gDirectory;
   TFile* fcor=TFile::Open("/scratchfs/lhaaso/hliu/temp_corr/20200229_1_SiPM_LED_BGM_correct_factor.root","READ");
   TTree* Ntree=(TTree*)fcor->Get("T_Signal");
   //Data data;
   //Ntree->SetBranchAddress("iTel",&(data.iTel));
   //Ntree->SetBranchAddress("T1MJD",&(data.T1MJD));
   //Ntree->SetBranchAddress("mAdcH_BC",&(data.mAdcH_BC));
   //Ntree->SetBranchAddress("mBaseH_C",&(data.mBaseH_C));
   //Ntree->SetBranchAddress("Temp_SiPM_C",&(data.Temp_SiPM_C));
   //Ntree->SetBranchAddress("SiPM_Temp_Factor",&(data.SiPM_Temp_Factor));
   //Ntree->SetBranchAddress("LED_DR_Temp_Factor",&(data.LED_DR_Temp_Factor));
   //Ntree->SetBranchAddress("table_Factor",&(data.table_Factor));
   //Ntree->SetBranchAddress("T1Door1Deg",&(data.T1Door1Deg));
   //Ntree->SetBranchAddress("T1Door2Deg",&(data.T1Door2Deg));
   //Ntree->SetBranchAddress("T1DledDrDStem",&(data.T1DledDrDStem));

   int Nentry=Ntree->GetEntries();
   double *NTime=new double[Nentry];
   double (*NAdcH)[1024]=new double[Nentry][1024];
   double (*NBaseH)[1024]=new double[Nentry][1024];
   double (*NTemp_SiPM)[1024]=new double[Nentry][1024];
   double (*NTemp_SiPM_Factor)[1024]=new double[Nentry][1024];
   double (*NTemp_LEDDR_Factor)[1024]=new double[Nentry][1024];
   double (*Ntable_Factor)[1024]=new double[Nentry][1024];
   double (*NBGM_Correct_Factor)[1024]=new double[Nentry][1024];
   double *NDoor1=new double[Nentry];
   double *NDoor2=new double[Nentry];
   double *NTemp_LEDDR=new double[Nentry];

   Data data;
   Ntree->SetBranchAddress("iTel",&(data.iTel));
   Ntree->SetBranchAddress("T1MJD",&(data.T1MJD));
   //Ntree->SetBranchAddress("mAdcH_BC",&(data.mAdcH_BC));
   //Ntree->SetBranchAddress("mBaseH_C",&(data.mBaseH_C));
   Ntree->SetBranchAddress("Temp_SiPM_C",&(data.Temp_SiPM_C));
   Ntree->SetBranchAddress("SiPM_Temp_Factor",&(data.SiPM_Temp_Factor));
   Ntree->SetBranchAddress("LED_DR_Temp_Factor",&(data.LED_DR_Temp_Factor));
   Ntree->SetBranchAddress("table_Factor",&(data.table_Factor));
   Ntree->SetBranchAddress("BGM_Correct_Factor",&(data.BGM_Correct_Factor));
   Ntree->SetBranchAddress("T1Door1Deg",&(data.T1Door1Deg));
   Ntree->SetBranchAddress("T1Door2Deg",&(data.T1Door2Deg));
   //Ntree->SetBranchAddress("T1DledDrDStem",&(data.T1DledDrDStem));
   for(int ii=0;ii<Nentry;ii++){
      Ntree->GetEntry(ii);
      NTime[ii]=(data.T1MJD-40587)*86400;
      NDoor1[ii]=data.T1Door1Deg;
      NDoor2[ii]=data.T1Door2Deg;
      NTemp_LEDDR[ii]=data.T1DledDrDStem;
      for(int jj=0;jj<1024;jj++){
         NAdcH[ii][jj]=data.mAdcH_BC[jj];
         NBaseH[ii][jj]=data.mBaseH_C[jj];
         NTemp_SiPM[ii][jj]=data.Temp_SiPM_C[jj];
         NTemp_SiPM_Factor[ii][jj]=data.SiPM_Temp_Factor[jj];
         NTemp_LEDDR_Factor[ii][jj]=data.LED_DR_Temp_Factor[jj];
         Ntable_Factor[ii][jj]=data.table_Factor[jj];
         NBGM_Correct_Factor[ii][jj]=data.BGM_Correct_Factor[jj];
      }
      //int sipmi=0;
      //printf("entry=%d Time=%.5lf door={%.1lf,%.1lf} Temp_LEDDR=%.2lf AdcH=%lf BaseH=%lf Temp_Sipm=%lf Temp_Sipm_Fact=%lf Temp_LEDDR_Fact=%lf table_Factor=%lf\n",ii,NTime[ii],NDoor1[ii],NDoor2[ii],NTemp_LEDDR[ii],NAdcH[ii][sipmi],NBaseH[ii][sipmi],NTemp_SiPM[ii][sipmi],NTemp_SiPM_Factor[ii][sipmi],NTemp_LEDDR_Factor[ii][sipmi],Ntable_Factor[ii][sipmi]);
   }
   fcor->Close();
   gdir->cd();

   int ITemp=0;
   const int nangle1=4;
   TH2F* ntotpe1[10][20][nangle1];
   for(int irot=0;irot<10;irot++){
      for(int itel=0;itel<20;itel++){
         for(int iazi=0;iazi<nangle1;iazi++){
            ntotpe1[irot][itel][iazi]=0;
            if(irot>=RotateDB::nrot) continue;
            if(itel>=RotateDB::ntel) continue;
            if(iazi==0&&(RotateDB::telindex[itel]!=5&&RotateDB::telindex[itel]!=6)) continue;
            if(iazi==1&&(RotateDB::telindex[itel]!=4&&RotateDB::telindex[itel]!=5)) continue;
            if(iazi==2&&(RotateDB::telindex[itel]!=3&&RotateDB::telindex[itel]!=4)) continue;
            if(iazi==3&&(RotateDB::telindex[itel]!=1&&RotateDB::telindex[itel]!=2&&RotateDB::telindex[itel]!=3)) continue;
            double Eref=RotateDB::GetHead()->GetEref(RotateDB::rotindex[irot],RotateDB::telindex[itel],1,iazi);
            double Erange[2]={Eref*0.7,Eref*1.3};
            ntotpe1[irot][itel][iazi]=new TH2F(Form("Rot%d_Tel%d_totpe_temp_Azi%d",RotateDB::rotindex[irot],RotateDB::telindex[itel],iazi),";Temperature [^{o}C];Total Npe [pe]",120,-30.,30,120,Erange[0],Erange[1]);
         }
      }
   }
   const int nangle2=6;
   TH2F* ntotpe2[10][20][nangle2];
   for(int irot=0;irot<10;irot++){
      for(int itel=0;itel<20;itel++){
         for(int iele=0;iele<nangle2;iele++){
            ntotpe2[irot][itel][iele]=0;
            if(irot>=RotateDB::nrot) continue;
            if(itel>=RotateDB::ntel) continue;
            double Eref=RotateDB::GetHead()->GetEref(RotateDB::rotindex[irot],RotateDB::telindex[itel],2,iele);
            double Erange[2]={Eref*0.7,Eref*1.3};
            ntotpe2[irot][itel][iele]=new TH2F(Form("Rot%d_Tel%d_totpe_temp_Ele%d",RotateDB::rotindex[irot],RotateDB::telindex[itel],iele),";Temperature [^{o}C];Total Npe [pe]",120,-30.,30,120,Erange[0],Erange[1]);
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
      //if(index!=221) continue;

      int itype=((index%100)/10);
      int iangle=(index%10);

      int size=pev->iSiPM.size();
      printf("LaserEvent2: entry=%d time=%d event=%d  index=%d size=%d\n",ientry,pev->rabbitTime,pev->iEvent,index,size);

      //find the index entry for the correction
      int findentry=-1;
      double mintimedis=1.0e10;
      for(int ii=0;ii<Nentry;ii++){
         double timedist=fabs((pev->rabbitTime+pev->rabbittime*2.0e-8)-NTime[ii]);
         if(timedist<mintimedis){
            findentry=ii;
            mintimedis=timedist;
         }
      }
      if(findentry>=Tmaxentry) continue;

      int ncontent;
      double error;
      double npe=pev->GetTotalPe(error,ncontent,pev->iTel,12,true);

      double temperature[5];
      bool res=RotateDB::GetHead()->GetEnv(pev->rabbitTime,RotateDB::rotindex[irot],temperature);
      if(!res) continue;

      if(itype==1&&ntotpe1[irot][itel][iangle]) ntotpe1[irot][itel][iangle]->Fill(temperature[ITemp],npe);
      if(itype==2&&ntotpe2[irot][itel][iangle]) ntotpe2[irot][itel][iangle]->Fill(temperature[ITemp],npe);
   }

   delete NTime;
   delete []NAdcH;
   delete []NBaseH;
   delete []NTemp_SiPM;
   delete []NTemp_SiPM_Factor;
   delete []NTemp_LEDDR_Factor;
   delete []Ntable_Factor;
   delete NDoor1;
   delete NDoor2;
   delete NTemp_LEDDR;

   fout->cd();
   for(int irot=0;irot<10;irot++){
      for(int itel=0;itel<20;itel++){
         for(int iazi=0;iazi<nangle1;iazi++){
            if(ntotpe1[irot][itel][iazi]) ntotpe1[irot][itel][iazi]->Write();
         }
      }
   }
   for(int irot=0;irot<10;irot++){
      for(int itel=0;itel<20;itel++){
         for(int iele=0;iele<nangle2;iele++){
            if(ntotpe2[irot][itel][iele]) ntotpe2[irot][itel][iele]->Write();
         }
      }
   }
   fout->Close();
   return 0;
}
