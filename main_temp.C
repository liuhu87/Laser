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
   TFile* fcor=TFile::Open("/scratchfs/lhaaso/hliu/temp_corr/20200229_1_SiPM_LED_correct_data_v2.root","READ");
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
   double *NDoor1=new double[Nentry];
   double *NDoor2=new double[Nentry];
   double *NTemp_LEDDR=new double[Nentry];

   Data data;
   Ntree->SetBranchAddress("iTel",&(data.iTel));
   Ntree->SetBranchAddress("T1MJD",&(data.T1MJD));
   Ntree->SetBranchAddress("mAdcH_BC",&(data.mAdcH_BC));
   Ntree->SetBranchAddress("mBaseH_C",&(data.mBaseH_C));
   Ntree->SetBranchAddress("Temp_SiPM_C",&(data.Temp_SiPM_C));
   Ntree->SetBranchAddress("SiPM_Temp_Factor",&(data.SiPM_Temp_Factor));
   Ntree->SetBranchAddress("LED_DR_Temp_Factor",&(data.LED_DR_Temp_Factor));
   Ntree->SetBranchAddress("table_Factor",&(data.table_Factor));
   Ntree->SetBranchAddress("T1Door1Deg",&(data.T1Door1Deg));
   Ntree->SetBranchAddress("T1Door2Deg",&(data.T1Door2Deg));
   Ntree->SetBranchAddress("T1DledDrDStem",&(data.T1DledDrDStem));
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
      }
      //int sipmi=0;
      //printf("entry=%d Time=%.5lf door={%.1lf,%.1lf} Temp_LEDDR=%.2lf AdcH=%lf BaseH=%lf Temp_Sipm=%lf Temp_Sipm_Fact=%lf Temp_LEDDR_Fact=%lf table_Factor=%lf\n",ii,NTime[ii],NDoor1[ii],NDoor2[ii],NTemp_LEDDR[ii],NAdcH[ii][sipmi],NBaseH[ii][sipmi],NTemp_SiPM[ii][sipmi],NTemp_SiPM_Factor[ii][sipmi],NTemp_LEDDR_Factor[ii][sipmi],Ntable_Factor[ii][sipmi]);
   }
   fcor->Close();
   gdir->cd();

   int IsHigh=-1;
   int IsSat=-1;
   // create histogram to save results
   char label[2][10]={"low","high"};
   char label2[2][10]={"sat","unsat"};
   TH2F* htemp_sgl[2][2][1024];
   TH2F* htemp_ave[2][2][1024];
   TH2F*  htemp_corr_sgl[2][2][1024];
   TH2F* htemp_corr2_sgl[2][2][1024];
   TH2F* hsum_temp_ave[2][2];
   for(int ishigh=0;ishigh<2;ishigh++){
      for(int issat=0;issat<2;issat++){
         for(int ii=0;ii<1024;ii++){
                  htemp_sgl[ishigh][issat][ii]=0;
                  htemp_ave[ishigh][issat][ii]=0;
             htemp_corr_sgl[ishigh][issat][ii]=0;
            htemp_corr2_sgl[ishigh][issat][ii]=0;
         }
         hsum_temp_ave[ishigh][issat]=0;
      }
   }

   for(int ishigh=0;ishigh<2;ishigh++){
      if(IsHigh>=0&&IsHigh!=ishigh) continue;
      int npebin=ishigh?400:900;
      double maxpe=ishigh?7500:33000;
      for(int issat=0;issat<2;issat++){
         if(IsSat>=0&&IsSat!=issat) continue;
         for(int ii=0;ii<1024;ii++){
                  htemp_sgl[ishigh][issat][ii] =new TH2F(Form("npe_temp_sgl_sipm%d_%s_%s",ii,label[ishigh],label2[issat]),";Temperature [^{o}C];Npe",100,-20,30,npebin,0,maxpe);
                  htemp_ave[ishigh][issat][ii] =new TH2F(Form("npe_temp_ave_sipm%d_%s_%s",ii,label[ishigh],label2[issat]),";Temperature [^{o}C];Npe",100,-20,30,npebin,0,maxpe);
             htemp_corr_sgl[ishigh][issat][ii]=new TH2F(Form("npe_corr1_sgl_sipm%d_%s_%s",ii,label[ishigh],label2[issat]),";Temperature [^{o}C];Npe",100,-20,30,npebin,0,maxpe);
            htemp_corr2_sgl[ishigh][issat][ii]=new TH2F(Form("npe_corr2_sgl_sipm%d_%s_%s",ii,label[ishigh],label2[issat]),";Temperature [^{o}C];Npe",100,-20,30,npebin,0,maxpe);
         }
         hsum_temp_ave[ishigh][issat]=new TH2F(Form("sum_npe_temp_ave_%s_%s",label[ishigh],label2[issat]),";Temperature [^{o}C];Npe",100,-20,30,500,1.0e3,1.0e6);
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
      if(index!=223) continue;

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

      double sipmpe[2][1024];
      bool sated[2][1024];
      for(int ii=0;ii<1024;ii++){
         sipmpe[0][ii]=0; sipmpe[1][ii]=0;
         sated[0][ii]=false; sated[1][ii]=false;
      }
      for(int ii=0;ii<size;ii++){
         sipmpe[0][pev->iSiPM.at(ii)]=pev->GetContent(ii,pev->iTel,4,true,true);
         sipmpe[1][pev->iSiPM.at(ii)]=pev->GetContent(ii,pev->iTel,3,true,true);
         sated[0][pev->iSiPM.at(ii)]=pev->GetContent(ii,pev->iTel,10,true,true)>0;
         sated[1][pev->iSiPM.at(ii)]=pev->GetContent(ii,pev->iTel,9,true,true)>0;
      }

      double temperature[1024];
      double temp_ave=0;
      int ntemp=0;
      TDirectory* gdir=gDirectory;
      char filename[200]="";
      bool exist=CommonTools::GetStatusFile(filename,(char*)pev->GetFileName());
      StatusDB::GetHead()->GetPreTemp(pev->iTel,pev->rabbitTime,0,exist?filename:0);
      for(int ii=0;ii<1024;ii++){
         temperature[ii]=StatusDB::GetHead()->PreTemp[ii];
         if(temperature[ii]<-100) continue;
         temp_ave+=temperature[ii];
         ntemp++;
      }
      if(ntemp>0) temp_ave/=ntemp;
      else temp_ave=-1000;
      if(gdir) gdir->cd();

      double npe_sum[2][2]={{0,0},{0,0}};
      for(int ii=0;ii<size;ii++){
         bool isclean=pev->CleanImage(ii,pev->iTel,true);
         //if(!isclean) continue;
         int sipm0=pev->iSiPM.at(ii);
         if(sipm0<0||sipm0>1023) continue;
         for(int ishigh=0;ishigh<2;ishigh++){
            if(IsHigh>=0&&(IsHigh!=ishigh)) continue;
            for(int issat=0;issat<2;issat++){
               if(IsSat>=0&&(IsSat!=issat)) continue;
               //if(issat&&(sated[ishigh][sipm0])) continue;
               if(issat==0){if(!sated[ishigh][sipm0]) continue;}
               if(issat==1){if(sated[ishigh][sipm0]) continue;}
               if(isclean) npe_sum[ishigh][issat]+=sipmpe[ishigh][sipm0];
               if(htemp_sgl[ishigh][issat][sipm0]) htemp_sgl[ishigh][issat][sipm0]->Fill(temperature[sipm0],sipmpe[ishigh][sipm0]);
               if(htemp_ave[ishigh][issat][sipm0]) htemp_ave[ishigh][issat][sipm0]->Fill(temp_ave,sipmpe[ishigh][sipm0]);

               CalibWFCTA::UseSiPMCalibVer=1;
               double npe_cor1=(temperature[sipm0]<-100)?sipmpe[ishigh][sipm0]:CalibWFCTA::GetHead()->DoCalibSiPM(pev->iTel,sipm0,sipmpe[ishigh][sipm0],temperature[sipm0],0x7);
               if(htemp_corr_sgl[ishigh][issat][sipm0]) htemp_corr_sgl[ishigh][issat][sipm0]->Fill(temperature[sipm0],npe_cor1);
               //CalibWFCTA::UseSiPMCalibVer=2;
               //double npe_cor2=(temp_ave<-100)?sipmpe[ishigh][sipm0]:CalibWFCTA::GetHead()->DoCalibSiPM(pev->iTel,sipm0,sipmpe[ishigh][sipm0],temp_ave,0x7);
               //if(htemp_corr2_sgl[ishigh][issat][sipm0]) htemp_corr2_sgl[ishigh][issat][sipm0]->Fill(temp_ave,npe_cor2);

               double npe_cor2=(findentry<0)?-1:(sipmpe[ishigh][sipm0]*Ntable_Factor[findentry][sipm0]);
               if(htemp_corr2_sgl[ishigh][issat][sipm0]) htemp_corr2_sgl[ishigh][issat][sipm0]->Fill(temperature[sipm0],npe_cor2);
               //double npe_cor2=(findentry<0)?-1:(sipmpe[ishigh][sipm0]*NTemp_SiPM_Factor[findentry][sipm0]);
               //if(htemp_corr2_sgl[ishigh][issat][sipm0]) htemp_corr2_sgl[ishigh][issat][sipm0]->Fill(temperature[sipm0],npe_cor2);
            }
         }
      }
      for(int ishigh=0;ishigh<2;ishigh++){
         if(IsHigh>=0&&(IsHigh!=ishigh)) continue;
         for(int issat=0;issat<2;issat++){
            if(IsSat>=0&&(IsSat!=issat)) continue;
            hsum_temp_ave[ishigh][issat]->Fill(temp_ave,npe_sum[ishigh][issat]);
         }
      }
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
   for(int ishigh=0;ishigh<2;ishigh++){
      for(int issat=0;issat<2;issat++){
         for(int ii=0;ii<1024;ii++){
            if(htemp_sgl[ishigh][issat][ii])             htemp_sgl[ishigh][issat][ii]->Write();
            if(htemp_ave[ishigh][issat][ii])             htemp_ave[ishigh][issat][ii]->Write();
            if(htemp_corr_sgl[ishigh][issat][ii])   htemp_corr_sgl[ishigh][issat][ii]->Write();
            if(htemp_corr2_sgl[ishigh][issat][ii]) htemp_corr2_sgl[ishigh][issat][ii]->Write();
         }
         if(hsum_temp_ave[ishigh][issat]) hsum_temp_ave[ishigh][issat]->Write();
      }
   }
   fout->Close();
   return 0;
}
