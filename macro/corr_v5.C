{
   int iTel=1;
   int iRot=2;
   //TFile* fin=TFile::Open(Form("/eos/user/h/hliu/LaserEvent/tel%d_v5.root",iTel));
   //TFile* fin=TFile::Open(Form("/scratchfs/ybj/lix/Laser/data_ylq/tel%d.root",iTel));
   //TFile* fin=TFile::Open(Form("/scratchfs/lhaaso/hliu/buffer2/tel%d.root",iTel));
   TFile* fin=TFile::Open(Form("/scratchfs/lhaaso/hliu/buffer/tel%d_1205_hliu_v2.2.root",iTel));
   TFile* fout=TFile::Open(Form("/eos/user/h/hliu/LaserEvent/tel%d_rot%d_hliu_1205.root",iTel,iRot),"RECREATE");

   for(int iazi=0;iazi<4;iazi++){
      TH2F* hist=(TH2F*)fin->Get(Form("Rot%d_Tel%d_Azi%d",iRot,iTel,iazi+1));
      TH1F* hcount=(TH1F*)fin->Get(Form("NAve_Rot%d_Tel%d_Azi%d",iRot,iTel,iazi+1));
      if(!hist) continue;
      if(hist->Integral()<10) continue;
      TH2F* hcorr=(TH2F*)hist->Clone(Form("Rot%d_Tel%d_Azi%d_corr",iRot,iTel,iazi+1));
      hcorr->Reset();
      for(int ibin=1;ibin<=hist->GetNbinsX();ibin++){
         double count,ecount;
         count=hcount->GetBinContent(ibin);
         ecount=hcount->GetBinError(ibin);
         if(count<=0) continue;
         for(int ibiny=0;ibiny<=hist->GetNbinsY()+1;ibiny++){
            hcorr->SetBinContent(ibin,ibiny,hist->GetBinContent(ibin,ibiny)/count);
            hcorr->SetBinError(ibin,ibiny,hist->GetBinError(ibin,ibiny)/count);
         }
      }
      TH1D* hy=hcorr->ProjectionX(Form("%s_px",hcorr->GetName()),0,hcorr->GetNbinsY()+1);
      fout->cd();
      hcorr->GetXaxis()->SetTimeDisplay(1);
      hcorr->GetXaxis()->SetNdivisions(-203);
      hcorr->GetXaxis()->SetTimeFormat("%Ss/%Mm/%Hh/%d/%m%F1970-01-01 00:00:00s0");
      hcorr->Write();
      hy->GetXaxis()->SetTimeDisplay(1);
      hy->GetXaxis()->SetNdivisions(-203);
      hy->GetXaxis()->SetTimeFormat("%Ss/%Mm/%Hh/%d/%m%F1970-01-01 00:00:00s0");
      hy->Write();
      hcount->GetXaxis()->SetTimeDisplay(1);
      hcount->GetXaxis()->SetNdivisions(-203);
      hcount->GetXaxis()->SetTimeFormat("%Ss/%Mm/%Hh/%d/%m%F1970-01-01 00:00:00s0");
      hcount->Write();
   }

   for(int iele=0;iele<6;iele++){
      TH2F* hist=(TH2F*)fin->Get(Form("Rot%d_Tel%d_Ele%d",iRot,iTel,iele+1));
      TH1F* hcount=(TH1F*)fin->Get(Form("NAve_Rot%d_Tel%d_Ele%d",iRot,iTel,iele+1));
      if(!hist) continue;
      if(hist->Integral()<10) continue;
      TH2F* hcorr=(TH2F*)hist->Clone(Form("Rot%d_Tel%d_Ele%d_corr",iRot,iTel,iele+1));
      hcorr->Reset();
      for(int ibin=1;ibin<=hist->GetNbinsX();ibin++){
         double count,ecount;
         count=hcount->GetBinContent(ibin);
         ecount=hcount->GetBinError(ibin);
         if(count<=0) continue;
         //printf("iele=%d ibinx=%d count=%lf\n",iele,ibin,count);
         for(int ibiny=0;ibiny<=hist->GetNbinsY()+1;ibiny++){
            hcorr->SetBinContent(ibin,ibiny,hist->GetBinContent(ibin,ibiny)/count);
            hcorr->SetBinError(ibin,ibiny,hist->GetBinError(ibin,ibiny)/count);
         }
      }
      TH1D* hy=hcorr->ProjectionX(Form("%s_px",hcorr->GetName()),0,hcorr->GetNbinsY()+1);
      TH1F* hcount2=(TH1F*)hcount->Clone(Form("NAve_Rot%d_Tel%d_Ele%d_corr",iRot,iTel,iele+1));
      hcount2->Reset();
      for(int ibin=1;ibin<=hcount->GetNbinsX();ibin++){
         double integ0=hcount->GetBinContent(ibin-1);
         double integ1=hcount->GetBinContent(ibin);
         double integ2=hcount->GetBinContent(ibin+1);
         if(ibin==1) integ0=0;
         if(ibin==hcount->GetNbinsX()) integ2=0;
         if(integ0>0&&integ1>0&&integ2>0) {printf("iele=%d bin=%d integral={%.2le,%.2le,%.2le} all exist\n",iele,ibin,integ0,integ1,integ2); continue;}
         if(integ1<integ0||integ1<integ2) {/*printf("iele=%d bin=%d integral={%.2le,%.2le,%.2le} not biggest\n",iele,ibin,integ0,integ1,integ2);*/ continue;}
         double content=hcount->GetBinContent(ibin);
         double econtent=hcount->GetBinError(ibin);
         if(ibin>1){
            content+=hcount->GetBinContent(ibin-1);
            econtent=sqrt(pow(econtent,2)+pow(hcount->GetBinError(ibin-1),2));
         }
         if(ibin<hist->GetNbinsX()){
            content+=hcount->GetBinContent(ibin+1);
            econtent=sqrt(pow(econtent,2)+pow(hcount->GetBinError(ibin+1),2));
         }
         hcount2->SetBinContent(ibin,content);
         hcount2->SetBinError(ibin,econtent);
      }
      fout->cd();
      hcorr->GetXaxis()->SetTimeDisplay(1);
      hcorr->GetXaxis()->SetNdivisions(-203);
      hcorr->GetXaxis()->SetTimeFormat("%Ss/%Mm/%Hh/%d/%m%F1970-01-01 00:00:00s0");
      hcorr->Write();
      hy->GetXaxis()->SetTimeDisplay(1);
      hy->GetXaxis()->SetNdivisions(-203);
      hy->GetXaxis()->SetTimeFormat("%Ss/%Mm/%Hh/%d/%m%F1970-01-01 00:00:00s0");
      hy->Write();
      //hcount2->GetXaxis()->SetTimeDisplay(1);
      //hcount2->GetXaxis()->SetNdivisions(-203);
      //hcount2->GetXaxis()->SetTimeFormat("%Ss/%Mm/%Hh/%d/%m%F1970-01-01 00:00:00s0");
      //hcount2->Write();

      hcount->GetXaxis()->SetTimeDisplay(1);
      hcount->GetXaxis()->SetNdivisions(-203);
      hcount->GetXaxis()->SetTimeFormat("%Ss/%Mm/%Hh/%d/%m%F1970-01-01 00:00:00s0");
      hcount->Write();
   }
   fout->Close();
}
