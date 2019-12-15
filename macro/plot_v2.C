{
   //gROOT->ForceStyle(0);
   gStyle->SetMarkerStyle(20);
   gStyle->SetMarkerSize(1.5);
   gStyle->SetLineWidth(3);

   int iTel=4;

   double xmin=1.0e12,xmax=-1.0e12,ymin=1.0e12,ymax=-1.0e12;
   //double xmin=1574510000,xmax=1574550000,ymin=0.1,ymax=3.0e6;
   TCanvas* cc=new TCanvas();
   //TPad* pad1=new TPad("pad1","",0,0,1,1);
   //TPad* pad2=new TPad("pad2","",0,0,1,1);
   //pad2 -> SetFillStyle(4000);    // Makes pad2 transparent

   //pad1->Draw();
   //pad1->SetLogy(1);
   //pad1->cd();
   TFile* fin=TFile::Open(Form("/eos/user/h/hliu/LaserEvent/tel%d_v2.root",iTel));
   TGraphErrors* gr[6]={0,0,0,0,0,0};
   TH1F* hdis[6]={0,0,0,0,0,0};
   double hrange[6][2]={{8.0e5,2.5e6},{2.0e5,1.4e6},{1.0e4,1.0e6},{1.0e4,8.0e5},{1.0e4,8.0e5},{1.0e4,8.0e5}};
   int color[]={1,2,3,4,6,9};
   int cindex=5;
   int nplot=0;
   for(int iele=0;iele<6;iele++){
      if(cindex>=0&&iele!=cindex) continue;
      TH2F* hist=(TH2F*)fin->Get(Form("Tel%d_Ele%d",iTel,iele+1));
      TH2F* NAve=(TH2F*)fin->Get(Form("NAve_Tel%d_Ele%d",iTel,iele+1));
      if((!hist)||(!NAve)) continue;
      if(hist->Integral()<=0) continue;
      gr[nplot]=new TGraphErrors();
      hdis[nplot]=new TH1F(Form("dis_Tel%d_Ele%d",iTel,iele+1),";Total Signal [pe];Entries",100,hrange[iele][0],hrange[iele][1]);
      for(int ibin=1;ibin<=hist->GetNbinsX();ibin++){
         double NN=NAve->GetBinContent(ibin);
         if(NN<=0) continue;
         double cont=0,econt=0;
         cont=hist->IntegralAndError(ibin,ibin,1,hist->GetNbinsY(),econt);
         if(cont<=0) continue;
         cont/=NN; econt/=NN;
         hdis[nplot]->Fill(cont);
         int np=gr[nplot]->GetN();
         double xx=hist->GetXaxis()->GetBinCenter(ibin);
         gr[nplot]->SetPoint(np,xx,cont);
         gr[nplot]->SetPointError(np,0,econt);
         if(np>=0){
         if(xx<xmin) xmin=xx;
         if(xx>xmax) xmax=xx;
         if(cont<ymin) ymin=cont;
         if(cont>ymax) ymax=cont;
         }
      }
      gr[nplot]->Draw(nplot==0?"apl":"pl");
      gr[nplot]->SetMarkerColor(color[iele]);
      gr[nplot]->SetLineColor(color[iele]);
      gr[nplot]->GetYaxis()->SetAxisColor( color[iele]);
      gr[nplot]->GetYaxis()->SetLabelColor(color[iele]);

      //hdis[nplot]->Draw(nplot==0?"hist":"hist same");
      hdis[nplot]->SetLineColor(color[iele]);

      nplot++;
   }
   for(int iele=0;iele<6;iele++){
      if(!gr[iele]) continue;
      gr[iele]->GetXaxis()->SetRangeUser(xmin,xmax);
      gr[iele]->GetYaxis()->SetRangeUser(ymin,ymax);
      gr[iele]->GetXaxis()->SetTimeDisplay(1);
      gr[iele]->GetXaxis()->SetNdivisions(-203);
      gr[iele]->GetXaxis()->SetTimeFormat("%Mm/%Hh/%d/%m%F1970-01-01 00:00:00s0");
   }
   gPad->Update();
   //gPad->SetLogy();

   /*TFile* fin2=TFile::Open("/eos/user/h/hliu/cloudmap_201910-12.root");
   TH1F* AveTemp=(TH1F*)fin2->Get(Form("AveTemp_Tel%d",iTel));
   TH1F* NAve=(TH1F*)fin2->Get(Form("NAve_%d",iTel));
   TH1F* MinTemp=(TH1F*)fin2->Get(Form("MinTemp_%d",iTel));
   int nbins=AveTemp?AveTemp->GetNbinsX():(MinTemp)?MinTemp->GetNbinsX():0;

   TGraphErrors* gr_ave=new TGraphErrors();
   TGraphErrors* gr_min=new TGraphErrors();

   double ymin2=1.0e12,ymax2=-1.0e12;
   int np=0;
   for(int ibin=1;ibin<=nbins;ibin++){
      double nn=(NAve)?NAve->GetBinContent(ibin):0;
      if(nn<=0) continue;
      if(AveTemp){
         double ave=AveTemp->GetBinContent(ibin);
         double xx=AveTemp->GetXaxis()->GetBinCenter(ibin);
         gr_ave->SetPoint(np,xx,ave/nn);
         if(ave/nn<ymin2) ymin2=ave/nn;
         if(ave/nn>ymax2) ymax2=ave/nn;
      }
      if(MinTemp) gr_min->SetPoint(np,MinTemp->GetXaxis()->GetBinCenter(ibin),MinTemp->GetBinContent(ibin));
      np++;
   }
   ymin2-=2;
   //ymax2+=2;
   //ymax2=-60;
   TGraphErrors* gr0=gr_ave;
   for(int ii=0;ii<gr0->GetN();ii++){
      double xx=gr0->GetX()[ii];
      double cont=gr0->GetY()[ii];
      double econt=gr0->GetEY()[ii];
      gr0->SetPoint(ii,gr0->GetX()[ii],ymin+(cont-ymin2)/(ymax2-ymin2)*(ymax-ymin));
      gr0->SetPointError(ii,0,(econt)/(ymax-ymin)*(ymax2-ymin2));
      if(xx<xmin) xmin=xx;
      if(xx>xmax) xmax=xx;
      //printf("ii=%d y1=%lf y2=%lf\n",ii,cont,ymin+(cont-ymin2)/(ymax2-ymin2)*(ymax-ymin));
   }
   //printf("yminmax1={%lf,%lf} yminmax2={%lf,%lf}\n",ymin,ymax,ymin2,ymax2);

   //pad2 -> SetFrameFillStyle(4000);    // Makes pad2 transparent
   //pad2 -> Draw();
   //pad2 -> cd();
   //gr_ave->Draw("Y+");
   gr0->Draw("pl");
   //gr0->GetXaxis()->SetTimeDisplay(1);
   //gr0->GetXaxis()->SetNdivisions(-203);
   //gr0->GetXaxis()->SetTimeFormat("%Mm/%Hh/%d/%m%F1970-01-01 00:00:00s0");

   // draw axis on the right side of the pad
   TGaxis *axis = new TGaxis(gPad->GetUxmax(),gPad->GetUymin(),gPad->GetUxmax(), gPad->GetUymax(),ymin2,ymax2,510,"+L");
   //TGaxis *axis = new TGaxis(xmax,gPad->GetUymin(),xmax, gPad->GetUymax(),ymin2,ymax2,510,"+L");
   axis -> Draw();*/
}
