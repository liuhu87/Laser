{
   gStyle->SetOptStat(0);
   gStyle->SetPadGridX(1);
   gStyle->SetPadGridY(1);
   //gStyle->SetOptLogy(1);

   int iTel=4;
   TFile* fin=TFile::Open(Form("/eos/user/h/hliu/LaserEvent/tel%d.root",iTel));
   int color[]={1,2,3,4,6,9};
   double elevation[]={10,20,30,40,50,55};
   TLegend* leg=new TLegend(0.7,0.1,0.9,0.4);
   leg->AddEntry((TObject*)0,Form("iTel=%d",iTel),"");
   int nplot=0;
   for(int iele=5;iele>=0;iele--){
      TH2F* hist=(TH2F*)fin->Get(Form("Tel%d_Ele%d",iTel,iele+1));
      if(!hist) continue;
      TH1D* hy=hist->ProjectionY(Form("ele%d",iele),1,hist->GetNbinsX());
      hy->SetLineColor(color[iele]);
      hy->SetLineWidth(3);
      hy->GetXaxis()->SetRangeUser(-8,8);
      hy->DrawNormalized(nplot==0?"hist":"hist same");
      hy->GetYaxis()->SetTitle("Normalized Entries");
      leg->AddEntry(hy,Form("Elevation=%.0lf",elevation[iele]),"l");
      nplot++;
   }
   leg->Draw("same");
}
