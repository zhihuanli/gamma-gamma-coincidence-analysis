//make delayed gamma-gamma asymptotic matrix using RADWARE approch.
//agg.C
#include <iostream>
#include <sstream>
#include "TString.h"
#include "TSpectrum.h"
#include "TMath.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TTree.h"
#include "TChain.h"

TString rootfile="../../data4/d46_125_all.root";//input
TString ggfile="d46_125gg.root";//output
TH2D *hggb,*hggmat;
void makeaggmat()
{
  cout<<"Generate gg matix to ["<<ggfile<<"] from ["<<rootfile<<"]"<<endl;
  TFile *fin=TFile::Open(rootfile);
  TTree *tree=(TTree*)fin->Get("tree");
  //for 123Ag,129Cd
  // TString scut0="dt0<300&&dt1-dt0<400";//cut from 2D gg.
  //TString scut1="dt0<300&&dt1-dt0>3000&&dt1-dt0<3400";//cut from 2D gg.
  //for 125Ag
  TString scut0="dt0<200&&dt1-dt0<300";//cut from 2D gg.
  TString scut1="dt0<200&&dt1-dt0>3500&&dt1-dt0<3800";//cut from 2D gg.
  // cout<<"Making gg histogram with condition of "<<scut<<endl;
  
  tree->Draw("cde0:cde1>>gg(4096,0,4096,4096,0,4096)",scut0,"colz");
  tree->Draw("cde0:cde1>>gg1(4096,0,4096,4096,0,4096)",scut1,"colz");
  auto hgg=(TH2D*)gROOT->FindObject("gg");
  auto hgg1=(TH2D*)gROOT->FindObject("gg1");
  // TH2D* hgg=new TH2D("gg","gg",4096,0,4096,4096,0,4096);
  hgg->Add(hgg,hgg1,1,-1);
  //auto hgg=(TH2D*)gROOT->FindObject("gg");
  auto hx=(TH1D*)hgg->ProjectionX("xTpj");

  TSpectrum *sx= new TSpectrum(500);
  Int_t nfoundx=sx->Search(hx,2,"",0.1);
  auto hbx=sx->Background(hx,10,"same");//8 for pd125,
  hbx->SetName("xTpjBg");hbx->SetTitle("xTpjBg");
  auto hpeakx=(TH2D*)hbx->Clone("xTpjPeak");
  hpeakx->Add(hx,hbx,1,-1);
  hpeakx->Draw("same");
  
  auto hy=(TH1D*)hgg->ProjectionY("yTpj");
  TSpectrum *sy= new TSpectrum(500);
  Int_t nfoundy=sy->Search(hy,2,"",0.1);
  auto hby=sy->Background(hy,15,"same");//8 for pd125,
  hby->SetName("yTpjBg");hby->SetTitle("yTpjBg");
  auto hpeaky=(TH2D*)hby->Clone("yTpjPeak");
  hpeaky->Add(hy,hby,1,-1);
  hpeaky->Draw("same");
  
  hggb=new TH2D("ggbmat","bgmat for gg",4096,0,4096,4096,0,4096);
  hggb->Reset();
  Double_t T,Pi,Pj,pi,pj,bi,bj,Bij,x,y;
  T=hx->Integral();
  for(int i=0;i<hgg->GetNbinsX();i++) {
    for(int j=0;j<hgg->GetNbinsY();j++) {
      // Pi=h->GetBinContent(i+1);
      //Pj=h->GetBinContent(j+1);
      pi=hpeakx->GetBinContent(i+1);
      pj=hpeaky->GetBinContent(j+1);
      bi=hbx->GetBinContent(i+1);
      bj=hby->GetBinContent(j+1);
      Bij=(bi*pj+bi*bj)/T;//asymetric matix
      x=hx->GetBinCenter(i+1);
      y=hy->GetBinCenter(j+1);
      hggb->Fill(x,y,Bij);
    }
  }
  hggmat=new TH2D("ggmat","gg with backsub",4096,0,4096,4096,0,4096);  
  hggmat->Add(hgg,hggb,1,-1);
 
  TFile *fout=new TFile(ggfile,"RECREATE");
  hgg->Write();
  hx->Write();
  hbx->Write();
  hpeakx->Write();
  hy->Write();
  hby->Write();
  hpeaky->Write();
  hggb->Write();
  hggmat->Write();
  fout->Write();
  fout->Close(); 
  
}

