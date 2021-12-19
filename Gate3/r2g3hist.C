// Make 3-fold gg-matrix(TH2) from ROOT files(TTree) 
// Author: Li Zhihuan, Peking University
// > root -l r2g3hist.C

#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"
#include "TString.h"
#include "TROOT.h"
#include "TSystem.h"
#include <iostream>
#include <ctime>

using namespace std;
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//range of runid
int RunIdStart = 5;
int RunIdStop =  78;//78
//input
TString fNameFormat = "./data/test%04d.root";//Path+filename, %04d: xxx0005.root for runid=5;
TString treeName="t";
//gamma Tree Branches in the input file
TString ghitBranch="nGammaOneEventAddBack";//hit
TString geBranch="nGammaEnergyAddBack";//ge[hit]
TString gtBranch="nGammaTimeAddBack";//gt[hit]

float dgt=18;//coincidence time window.
//output
int nBin = 2000;
int MaxE = 2000;
TString fOutName="g3hist.root";

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
TH1I *h3x;
TH2I *h3xy;
TH2I *hh[2000];

clock_t start,stop;

int root2g3hist(int run);

void r2g3hist()
{
    start = clock();

    cout<<"Creating Histograms ..."<<endl;
    h3x=new TH1I("h3x","h3x",nBin,0,MaxE);
    h3xy=new TH2I("h3xy","h3xy",nBin,0,MaxE,nBin,0,MaxE);
    for(int i=0;i<nBin;i++) 
      hh[i]=new TH2I(Form("h3xy%04d",i),"",nBin,0,MaxE,nBin,0,MaxE);
    
    for(int i=RunIdStart;i<=RunIdStop;i++)
        root2g3hist(i);//tree to TH

    TFile *fout = new TFile(fOutName.Data(),"recreate");//output file 
    h3x->Write();
    h3xy->Write();
    for(int i=0;i<nBin;i++) {
      hh[i]->Write();
      delete hh[i];
      if(i%20==0) cout<<".";
    }
    cout<<endl;
    fout->Close();
    cout<<"Write Histgrams to "<<fOutName.Data()<<endl;

    stop=clock();

    double seconds=(double)(stop-start)/CLOCKS_PER_SEC;
    cout<<"Time taken by program is : "<<seconds<<" seconds."<<endl;
}

int root2g3hist(int run)
{
  if(gSystem->AccessPathName(Form(fNameFormat.Data(),run))) 
    {
      cout <<Form(fNameFormat.Data(),run)<< " does not exist!" <<endl;
      return 0;
    }
    start = clock();

    TFile *f=new TFile(Form(fNameFormat.Data(),run)); 
    TTree *tree=(TTree*) f->Get(treeName.Data());
    int ghit;
    double ge[1000];
    Long64_t gt[1000];
    tree->SetBranchAddress(ghitBranch.Data(),&ghit);//hit
    tree->SetBranchAddress(geBranch.Data(),&ge);//energy
    tree->SetBranchAddress(gtBranch.Data(),&gt);//time    
    Long64_t nentries=tree->GetEntries();

    for(Long64_t jentry=0;jentry<nentries;jentry++)  {
        tree->GetEntry(jentry);
        if(ghit<2) continue;
        for(int i=0;i<ghit;i++) {
            for(int j=0;j<ghit;j++) {
                for(int k=0;k<ghit;k++) {
                    if(i==j || j==k || k==i) continue;
                    if(abs(gt[i]-gt[j])>dgt ) continue;
                    if(abs(gt[j]-gt[k])>dgt ) continue;
                    if(abs(gt[i]-gt[k])>dgt ) continue;
                    if(ge[i]<20 || ge[i]>MaxE) continue;
                    if(ge[j]<20 || ge[j]>MaxE) continue;
                    if(ge[k]<20 || ge[k]>MaxE) continue;
                    int ibin=h3x->FindBin(ge[i]);
                    if(ibin>=1 && ibin<=2000){
                        h3x->Fill(ge[j]);
                        h3xy->Fill(ge[j],ge[k]);
                        hh[ibin-1]->Fill(ge[j],ge[k]);
                    }
                    
                }
            }
        }

    }
    stop=clock();

    double seconds=(double)(stop-start)/CLOCKS_PER_SEC;
    cout<<run<<", "<<nentries<<", "<<seconds<<" seconds."<<endl;
    return 1;
}
