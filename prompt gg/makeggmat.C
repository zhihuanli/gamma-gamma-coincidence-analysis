//Make prompt g-g coincidence matrix using RADWARE approach.
//Usage: root -l makeggmat.C
//The output file will be analyzed by gg.C
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

TString rootfile="../../decay46_123_all.root"; // Name of input file
TString ggfile="46_123gg300s.root"; // Name of output file
int nebin=2500;//number of bin for TH2I
double emax=2500;//maximum energy .
void makeggmat()
{

  cout<<"Generate gg matix to ["<<ggfile<<"] from ["<<rootfile<<"]"<<endl;
  TFile *fin=TFile::Open(rootfile);
  TTree *tree=(TTree*)fin->Get("tree");
  //make gg 2D histogram
  TString atcut="caxt>0&&caxt<1000 && cayt>0&&cayt<1000 && abs(caxt-cayt)<200";
  TString sgtree=Form("caxe:caye>>gg(%i,0,%f,%i,0,%f)",nebin,emax,nebin,emax);
  TString sgcut= Form("%s && decaytime<300",atcut.Data());
  TString sbtree=Form("caxe:caye>>bg0(%i,0,%f,%i,0,%f)",nebin,emax,nebin,emax);
  TString sbcut= Form("%s && decaytime<4000 && decaytime>4300",atcut.Data());  
  tree->Draw(sgtree,sgcut);
  auto hgg=(TH2I*)gROOT->FindObject("gg"); //gg matrix
  tree->Draw(sbtree,sbcut);  
  auto hbg0=(TH2I*)gROOT->FindObject("bg0"); //bgg matrix
  hgg->Add(hgg,hbg0,1,-1);
  auto hx=(TH1D*)hgg->ProjectionX("Tpj"); //total projection spectrum(Tpj)
  TSpectrum *sx= new TSpectrum(500);
  Int_t nfoundx=sx->Search(hx,2,"",0.1);
  auto hbx=sx->Background(hx,10,"same"); //background of Tpj
  hbx->SetName("TpjBg");hbx->SetTitle("TpjBg");
  auto hpeakx=(TH2I*)hbx->Clone("TpjPeak");
  hpeakx->Add(hx,hbx,1,-1); //net peaks of Tpj
  hpeakx->Draw("same");

  TH2I* hggb=new TH2I("ggbmat","bgmat for gg",nebin,0,emax,nebin,0,emax);
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
      hggb->Fill(x,y,Bij);// backgound gg matix
    }
  }
  TH2I* hggmat=new TH2I("ggmat","gg with backsub",nebin,0,emax,nebin,0,emax);
  hggmat->Add(hgg,hggb,1,-1);// background subtracted gg matix.
  TFile *fout=new TFile(ggfile,"RECREATE");
  hgg->Write();//"gg"- gg matix 
  hx->Write();//"Tpj" -total projection spectrum
  hbx->Write();//"TpjBg" - background of Tpj
  hpeakx->Write();//"TpjPeak" -net peaks of Tpj
  hggb->Write();//"ggbmat" - background gg matix
  hggmat->Write();//"ggmat" -background subtracted gg matix 
  fout->Write();
  fout->Close(); 
 
}

