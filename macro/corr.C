{
   int iTel=6;
   TFile* fin=TFile::Open(Form("/eos/user/h/hliu/LaserEvent/tel%d.root",iTel));
   TFile* fout=TFile::Open(Form("/eos/user/h/hliu/LaserEvent/tel%d_corr.root",iTel),"RECREATE");

   for(int iele=0;iele<6;iele++){
      TH2F* hist=(TH2F*)fin->Get(Form("Tel%d_Ele%d",iTel,iele+1));
      if(!hist) continue;
      if(hist->Integral()<10) continue;
      TH2F* hcorr=(TH2F*)hist->Clone(Form("Tel%d_Ele%d_corr",iTel,iele+1));
      hcorr->Reset();
      for(int ibin=1;ibin<=hist->GetNbinsX();ibin++){
         double integ0=hist->Integral(ibin-1,ibin-1,0,hist->GetNbinsY()+1);
         double integ1=hist->Integral(ibin,ibin,0,hist->GetNbinsY()+1);
         double integ2=hist->Integral(ibin+1,ibin+1,0,hist->GetNbinsY()+1);
         if(ibin==1) integ0=0;
         if(ibin==hist->GetNbinsX()) integ2=0;
         if(integ0>0&&integ1>0&&integ2>0) {printf("iele=%d bin=%d integral={%.2le,%.2le,%.2le} all exist\n",iele,ibin,integ0,integ1,integ2); continue;}
         if(integ1<integ0||integ1<integ2) {/*printf("iele=%d bin=%d integral={%.2le,%.2le,%.2le} not biggest\n",iele,ibin,integ0,integ1,integ2);*/ continue;}
         for(int ibiny=0;ibiny<=hist->GetNbinsY()+1;ibiny++){
            double content=hist->GetBinContent(ibin,ibiny);
            double econtent=hist->GetBinError(ibin,ibiny);
            if(ibin>1){
               content+=hist->GetBinContent(ibin-1,ibiny);
               econtent=sqrt(pow(econtent,2)+pow(hist->GetBinError(ibin-1,ibiny),2));
            }
            if(ibin<hist->GetNbinsX()){
               content+=hist->GetBinContent(ibin+1,ibiny);
               econtent=sqrt(pow(econtent,2)+pow(hist->GetBinError(ibin+1,ibiny),2));
            }
            hcorr->SetBinContent(ibin,ibiny,content);
            hcorr->SetBinError(ibin,ibiny,econtent);
         }
      }
      fout->cd();
      hcorr->Scale(1./26);
      hcorr->Write();
   }
   fout->Close();
}
