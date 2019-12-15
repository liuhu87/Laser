{
   //gStyle->SetOptStat(0);
   //TH1F* h1=new TH1F("h1","",100,-10,10);
   //h1->FillRandom("gaus",10000);
   //h1->SetLineColor(2);
   //h1->SetLineWidth(3);
   //TH1F* h2=new TH1F("h2","",100,-10,10);
   //h2->FillRandom("gaus",100000);
   //h2->SetLineColor(4);
   //h2->SetLineWidth(3);

   double xmax=100;
   TF1* a=new TF1("a","x",0,xmax);
   a->SetLineColor(2);
   TF1* b=new TF1("b","TMath::Exp(-x/10)",0,xmax);
   b->SetLineColor(4);

   TCanvas* cc=new TCanvas();
   TPad* pad1=new TPad("pad1","",0,0,1,1);
   TPad* pad2=new TPad("pad2","",0,0,1,1);

   /*pad1->SetLogy(1);
   pad1->Draw();
   pad1->cd();
   //h1->Draw();
   a->Draw("A");
   a->GetYaxis()->SetRangeUser(.1,100);
   gPad->Update();

   pad2->SetLogy(1);
   pad2->SetFillStyle(4000);
   pad2->SetFrameFillStyle(0);
   //pad2->SetFrameFillStyle(4000);
   pad2->Draw();
   //h2->Draw("Y+");
   b->Draw("Y+");
   b->GetYaxis()->SetAxisColor(kRed);
   b->GetYaxis()->SetLabelColor(kRed);*/

   a->Draw("AL");
   gPad->Update();
   b->Draw("SAME");
}
