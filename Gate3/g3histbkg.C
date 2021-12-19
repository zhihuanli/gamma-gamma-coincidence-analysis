// Generate background spectrum and gg-matrix using Radware approch.
// Author: Li Zhihuan, Peking University

//> root -l gate3bkg.C

// (1).If bkg spectrum looks OK, run write().
// (2). Otherwise run genBackground(res) with new res value until the (1) is meet.

#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"
#include "TString.h"
#include "TROOT.h"
#include "TSpectrum.h"
#include <iostream>
#include <ctime>
#define RESET   "\033[0m"
#define RED     "\033[31m"      /* Red */
using namespace std;

//input 
TString fInName = "g3hist.root";//root file 
TString h2name = "h3xy";//xy projection 
TString h1name = "h3x"; // x projection 
//output 
TString fOutName = "g3histbkg.root";//proj root file
TString h1bname="h3xb";//background of h3x;

////////////////////////////////////////////////////////
TH2I *hg3xy;
TH1I *hg3x;
TH1I *hg3xb;
TSpectrum *sa;
void genBackground(int res=20);

void g3histbkg()
{
  TFile *fin = new TFile(fInName.Data());
  if ((!fin) || (fin->IsZombie())) {
    cout<<fInName.Data()<<" does not exist!"<<endl;
    return;
  }
  hg3xy = (TH2I*)fin->Get(h2name.Data())->Clone(h2name.Data()); 
  hg3x = (TH1I*)fin->Get(h1name.Data())->Clone(h1name.Data());
  sa = new TSpectrum(1000);
  genBackground();

}
void genBackground(int res)
{
  if(hg3xb) hg3xb->Clear();
  hg3xb=(TH1I*)sa->Background(hg3x,res,"nosmoothing");
  hg3xb->SetName("h3xb");
  hg3xb->SetTitle("h3xb");
  hg3x->Draw();
  hg3xb->Draw("same");

}

void write()
{
  //output to ROOT file
  TFile *fout=new TFile(fOutName.Data(),"RECREATE");//user
  hg3xy->Write();
  hg3x->Write();
  hg3xb->Write();
  fout->Close();
  cout<<"Write Histgrams hg3xy, hg3x and hg3xb to "<<fOutName.Data()<<endl;
}
