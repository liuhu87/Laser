#include "EveNtuple.h"

#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TH2.h"
#include "TH1.h"
#include "TGraph.h"
#include "TString.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "WFCTAEvent.h"

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <cstring>
#include "stdint.h"
using namespace std;

int iTel;
const char* outname;
bool doclean=true;
int timefirst;
int timelast;
TFile* outfile=0;
int main(int argc, char ** argv){
   gStyle->SetNumberContours(50);
   gStyle->SetOptStat(0);
   gStyle->SetOptLogy(0);
   gStyle->SetOptLogz(0);
   gStyle->SetPadGridX(1);
   gStyle->SetPadGridY(1);
   gStyle->SetPalette(1);

    // arguments
    if (argc<4) {
       printf("   Usage %s <RunList> <iTel> <out_name> <First> <Last> <MaxEvents> <FirstEvent>\n",argv[0]);
       exit(0);
    }
    // vars 
    const char* runlist = argv[1];
    iTel=atoi(argv[2]);
    outname = argv[3];
    char cdir[200]="/afs/ihep.ac.cn/users/h/hliu/Documents/Analysis/LaserEvent";
    int first,last;
    int nlineall;
    if(strstr(runlist,".root")){
       nlineall=1;
       timefirst=CommonTools::GetTimeFromFileName(runlist)-600;
       timelast=CommonTools::GetTimeFromFileName(runlist)+3600+600;
       first=0;
       last=0;
    }
    else{
       char firstline[300];
       char lastline[300];
       nlineall=CommonTools::GetFirstLastLine(Form("%s/%s",cdir,runlist),firstline,lastline);
       timefirst=CommonTools::GetTimeFromFileName(firstline)-600;
       timelast=CommonTools::GetTimeFromFileName(lastline)+600;
       printf("firstline=%s\n",firstline);
       printf("lastline =%s\n",lastline);
       first=argc>4?atoi(argv[4]):0;
       last=argc>5?atoi(argv[5]):(nlineall-1);
    }
    printf("nline=%d timefirst=%d timelast=%d\n",nlineall,timefirst,timelast);
    int maxentries = argc>6?atoi(argv[6]):-1;
    int firstevt = argc>7?atoi(argv[7]):0;
    cout<< runlist << "  " << outname << "  " << first << "  " << last << "  " << maxentries << "  " << firstevt <<endl;

    //get event ntuple
    TChain* eve_chain=new TChain("laserevent");
    if(strstr(runlist,".root")){
       eve_chain->Add(runlist);
    }
    else{
       //eve_chain->AddFromFile(Form("/afs/ihep.ac.cn/users/h/hliu/Documents/Analysis/LaserEvent/%s",runlist),first,last+1);
       std::ifstream fin(Form("%s/%s",cdir,runlist),std::ios::in);
       int nline=0;
       int nadded=0;
       char buff[500];
       fin.getline(buff,500);
       nline++;
       while(fin.good()){
          if( ((first>=0&&last>=first)&&(nline-1>=first&&nline-1<=last)) ||(!(first>=0&&last>=first)) ){
             eve_chain->Add(buff);
             nadded++;
             printf("Adding %d(%d): %s\n",nadded,nline,buff);
          }
          else if((first>=0&&last>=first)&&(nline-1>last)) break;
          fin.getline(buff,500);
          nline++;
       }
    }

    int entries_chain=eve_chain->GetEntries();

    //output
    char OutType[20];
    int length=strlen(outname);
    int iloc=-1;
    for(int ii=length-1;ii>=0;ii--){
       if(outname[ii]=='.'){ iloc=ii; break;}
    }
    for(int ii=0;ii<length;ii++){
       if(ii<iloc) ;//filename[ii]=outname[ii];
       else if(ii==iloc) ;//filename[ii]='\0';
       else OutType[ii-iloc-1]=outname[ii];
    }
    OutType[length-1-iloc]='\0';
    if(strstr(OutType,"root")) outfile=TFile::Open(outname,"RECREATE");

    EveNtuple* eve_selector=new EveNtuple();

    //dump
    printf("EveNtuple with %d entries.\n",entries_chain);

    //check
    if (entries_chain<=0) {
       printf("Nothing done (EveNtuple with %d entries).\n",entries_chain);
       if(outfile){
          //outfile->Write();
          printf("Save histograms in %s\n",outfile->GetName());
          outfile->Close();
       }
       return 0;
    }

    if(maxentries<=0) maxentries=entries_chain;
    TH1::SetDefaultSumw2();

    // process
    cout<<"Processing ..."<<endl;
    eve_chain->Process(eve_selector,"",maxentries,firstevt);

    // save
    if(outfile){
       //outfile->Write();
       printf("Save histograms in %s\n",outfile->GetName());
       outfile->Close();
    }
    else{
       TCanvas* cc=new TCanvas();
       cc->Print(Form("%s)",outname),OutType);
    }

    return 0;
}
