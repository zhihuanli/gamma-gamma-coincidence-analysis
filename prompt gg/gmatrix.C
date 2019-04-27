//Make prompt g-g,g-g-g coincidence matrix using RADWARE approach.
//The output file will be analyzed by g2.C && g3.C
// Author: Zhihuan Li
// Email: zhli@pku.edu.cn
// Last-Updated: Apr.2019

#define gmatrix_cxx
#include "gmatrix.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

void gmatrix::Loop()
{
  //matrix setting
  Double_t gmin=0;
  Double_t gmax=1500;
  int ngbin=1500;

  //for two-fold
  TH2F *hg2xy=new TH2F("hg2xy","g-g matrix",ngbin,gmin,gmax,ngbin,gmin,gmax);//for two-fold
  TH1F *hg2x=new TH1F("hg2x","total projection spectrum for hg2xy",ngbin,gmin,gmax);
  TH1F *hg2xp=new TH1F("hg2xp","net peaks of total projection spectrum of hg2xy",ngbin,gmin,gmax);
  TH1F *hg2xb;

  //for tree-fold
  Int_t bins[3] = {ngbin, ngbin, ngbin};
  Double_t xmin[3] = {gmin, gmin, gmin};//must be Double_t, Float_t will casue error
  Double_t xmax[3] = {gmax, gmax, gmax};
  
  THnSparse *hg3xyz;//Sparse matrix.
  hg3xyz=new THnSparseF("hg3xyz", "g-g-g matrix", 3, bins, xmin, xmax);//for tree-fold
  TH2F *hg3xy=new TH2F("hg3xy","x-y projection of hg3",ngbin,gmin,gmax,ngbin,gmin,gmax);
  TH1F *hg3x=new TH1F("hg3x","total projection spectrum for hg3xyz",ngbin,gmin,gmax);
  TH1F *hg3xp=new TH1F("hg3xp","net peaks of total projection spectrum of hg3xyz",ngbin,gmin,gmax);
  TH1F *hg3xb;

  TSpectrum *sa=new TSpectrum(200);
  
  TFile *fout=new TFile("gamma.root","RECREATE");
   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;
          //for two-fold
    if(ahit>1) {
      for(int i=0; i<ahit; i++) {
        for(int j=0; j<ahit; j++) {
	  bool bij=abs(at[i]-at[j])>200; //prompt coincidence time window
	    if(bij) continue;
	    if(i==j) continue;
	    hg2x->Fill(ae[i]);
	    hg2xy->Fill(ae[i],ae[j]);
	}
      }
    }
    //for tree-fold
    if(ahit>2) {
      for(int i=0; i<ahit; i++) {
        for(int j=0; j<ahit; j++) {
	  for(int k=0; k<ahit; k++) {
	    bool bij=abs(at[i]-at[j])>200;
	    bool bik=abs(at[i]-at[k])>200;
	    bool bjk=abs(at[i]-at[j])>200;
	    if(bij || bik || bjk) continue;
	    if(i==j || i==k || j==k) continue;
	    hg3x->Fill(ae[i]);
	    hg3xy->Fill(ae[i],ae[j]);
	    Double_t x[3]={ae[i],ae[j],ae[k]};
	    hg3xyz->Fill(x);
	    
	  }
	}
      }
    }
    if(jentry%500000==0) cout<<jentry<<endl;;
   }
   //"nosmoothing" option to avoid overestimate the background
   hg2xb=(TH1F*)sa->Background(hg2x,5,"nosmoothing");
   hg2xb->SetName("hg2xb");
   hg2xb->SetTitle("background of total projection spectrum of hg2xy");
   hg2xp->Add(hg2x,hg2xb,1,-1);
 
   hg3xb=(TH1F*)sa->Background(hg3x,5,"nosmoothing same");
   hg3xb->SetName("hg3xb");
   hg3xb->SetTitle("background of total projection spectrum of hg3xy");
   hg3xp->Add(hg3x,hg3xb,1,-1);
   
   hg2xy->Write();
   hg2x->Write();
   hg2xb->Write();
   hg2xp->Write();

   hg3xyz->Write();
   hg3xy->Write();
   hg3x->Write();
   hg3xb->Write();
   hg3xp->Write();


   fout->Close();
}
