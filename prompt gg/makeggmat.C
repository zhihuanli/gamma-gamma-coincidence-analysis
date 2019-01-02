//make prompt g-g coincidence matrix.
//root -l gg.C
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

TString rootfile="data_R0068_m400.root";//name of input ROOT file
TString ggfile="xia.root";//name of output file
void makeggmat()
{
  cout<<"Generate gg matix to ["<<ggfile<<"] from ["<<rootfile<<"]"<<endl;
  TFile *fin=TFile::Open(rootfile);
  // TTree *tree=(TTree*)fin->Get("tree");
  //condition for gg matrix 
  //TString scut0="abs(caxt-cayt)<200&&decaytime<200";  
  //make gg 2D histogram 
  //tree->Draw("caxe:caye>>gg(4096,0,4096,4096,0,4096)",scut0,"colz");
  auto hgg=(TH2I*)gROOT->FindObject("matrix"); //gg matrix
  auto hx=(TH1D*)hgg->ProjectionX("Tpj"); //total projection spectrum(Tpj)
  TSpectrum *sx= new TSpectrum(500);
  Int_t nfoundx=sx->Search(hx,2,"",0.1);
  auto hbx=sx->Background(hx,8,"same"); //background of Tpj
  hbx->SetName("TpjBg");hbx->SetTitle("TpjBg");
  auto hpeakx=(TH2I*)hbx->Clone("TpjPeak");
  hpeakx->Add(hx,hbx,1,-1); //net peaks of Tpj
  hpeakx->Draw("same");

  TH2I* hggb=new TH2I("ggbmat","bgmat for gg",4096,0,3072,4096,0,3072);
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
  TH2I* hggmat=new TH2I("ggmat","gg with backsub",4096,0,3072,4096,0,3072);
  
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

