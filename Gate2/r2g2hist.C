// Make 2-fold gg-matrix(TH2) from ROOT files(TTree) 
// Author: Li Zhihuan, Peking University

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
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.

//range of runid
int RunIdStart = 5;
int RunIdStop = 78;
//input
TString fNameFormat = "./data/test%04d.root";//Path+filename, %04d: xxx0005.root for runid=5;
TString treeName="t";
//gamma Tree Branches in the input file
TString ghitBranch="nGammaOneEventAddBack";//hit
TString geBranch="nGammaEnergyAddBack";//ge[hit]
TString gtBranch="nGammaTimeAddBack";//gt[hit]

float dgt=18;//coincidence time window.
//output
int nBin = 4000;
int MaxE = 4000;
TString fOutName="g2hist.root";

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.
TH1I *h2x;
TH2I *h2xy;
clock_t start,stop;

int root2g2hist(int run);

void r2g2hist()
{

    start = clock();
    h2x=new TH1I("h2x","h2x",nBin,0,MaxE);
    h2xy=new TH2I("h2xy","h2xy",nBin,0,MaxE,nBin,0,MaxE);
    for(int i=RunIdStart;i<=RunIdStop;i++)
        root2g2hist(i);//tree to TH

    TFile *fout = new TFile(fOutName.Data(),"recreate");//output file 
    h2x->Write();
    h2xy->Write();
    fout->Close();
    cout<<"Write Histgrams to "<<fOutName.Data()<<endl;

    stop=clock();
    double seconds=(double)(stop-start)/CLOCKS_PER_SEC;
    cout<<"Time taken by program is : "<<seconds<<" seconds."<<endl;
}

int root2g2hist(int run)
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
    cout<<run<<","<<nentries<<endl;
    for(Long64_t jentry=0;jentry<nentries;jentry++)  {
        tree->GetEntry(jentry);
        if(ghit<1) continue;
        for(int i=0;i<ghit;i++) {
            for(int j=0;j<ghit;j++) {
                if(i==j) continue;
                if(abs(gt[i]-gt[j])>dgt ) continue;
                if(ge[i]<20 || ge[i]>MaxE) continue;
                if(ge[j]<20 || ge[j]>MaxE) continue;
                h2x->Fill(ge[i]);
                h2xy->Fill(ge[i],ge[j]);    
            }
        }
    }

    double seconds=(double)(stop-start)/CLOCKS_PER_SEC;
    cout<<run<<", "<<nentries<<", "<<seconds<<" seconds."<<endl;
    return 1;
}
