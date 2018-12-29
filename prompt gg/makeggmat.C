//make prompt g-g coincidence matrix.
//gg.C
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

TString rootfile="../../decay46_123_all.root";//input
TString ggfile="43_123gg.root";//output
void makeggmat(TString rootfile="../decay46_123_all.root")
{
  cout<<"Generate gg matix to ["<<ggfile<<"] from ["<<rootfile<<"]"<<endl;
  TFile *fin=TFile::Open(rootfile);
  TTree *tree=(TTree*)fin->Get("tree");

  TString scut0="abs(caxt-cayt)<200&&decaytime<200";//set condition for gg.  
  tree->Draw("caxe:caye>>gg(4096,0,4096,4096,0,4096)",scut0,"colz");//gg coin.
  auto hgg=(TH2D*)gROOT->FindObject("gg");
  auto hx=(TH1D*)hgg->ProjectionX("Tpj");
  TSpectrum *sx= new TSpectrum(500);
  Int_t nfoundx=sx->Search(hx,2,"",0.1);
  auto hbx=sx->Background(hx,8,"same");//
  hbx->SetName("TpjBg");hbx->SetTitle("TpjBg");
  auto hpeakx=(TH2D*)hbx->Clone("TpjPeak");
  hpeakx->Add(hx,hbx,1,-1);
  hpeakx->Draw("same");

  TH2D* hggb=new TH2D("ggbmat","bgmat for gg",4096,0,4096,4096,0,4096);
  hggb->Reset();
  Double_t T,Pi,Pj,pi,pj,Bij,x,y;
  T=hx->Integral();
  for(int i=0;i<hgg->GetNbinsX();i++) {
    for(int j=0;j<hgg->GetNbinsY();j++) {
      Pi=hx->GetBinContent(i+1);
      Pj=hx->GetBinContent(j+1);
      pi=hpeakx->GetBinContent(i+1);
      pj=hpeakx->GetBinContent(j+1);
      Bij=(Pi*Pj-pi*pj)/T;
      x=hx->GetBinCenter(i+1);
      y=hx->GetBinCenter(j+1);
      hggb->Fill(x,y,Bij);
    }
  }
  TH2D* hggmat=new TH2D("ggmat","gg with backsub",4096,0,4096,4096,0,4096);
  
  hggmat->Add(hgg,hggb,1,-1);
  TFile *fout=new TFile(ggfile,"RECREATE");
  hgg->Write();
  hx->Write();
  hbx->Write();
  hpeakx->Write();
  hggb->Write();
  hggmat->Write();
  fout->Write();
  fout->Close(); 
}

