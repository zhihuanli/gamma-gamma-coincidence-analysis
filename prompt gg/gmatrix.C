//Branch in gammaX.root files
//ahit: number of hit in a event (after addback for Clover);
//aid[ahit]:detector id
//ae[ahit]: energy
//at[ahit]: time


//1.Create gmatix.C
//root -l
//root [0] TChain ch("tree");
//root [1] ch.Add("gamma1.root");//Add all your gamma files to ch
//root [2] ch.Add("gamma2.root");
//... ...
//root [9] ch->MakeClass("gmatix"); //create gmatrix.C & gmatrix.h
//root [10].q

//2.modifiy gmatrix.C according to your experimental condition:
//3.make matrix data gamma.root
//root -l
//root [0] .L gmatrix.C
//root [1] gmatrix t
//root [2] t.Loop();
//root [3] .q



#define gmatrix_cxx
#include "gmatrix.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

void gmatrix::Loop()
{
  //matrix setting
  Double_t gmin=0;
  Double_t gmax=3000;
  int ngbin=3000;
  //for all
  TH1F *hg1x=new TH1F("hg1x","inclusive gamma spectrum",ngbin,gmin,gmax);
  TH1F *hg1xp=new TH1F("hg1xp","net peaks of inclusive gamma spectrum",ngbin,gmin,gmax);
  TH1F *hg1xb;
  
  //for two-fold
  TH2F *hg2xy=new TH2F("hg2xy","g-g matrix",ngbin,gmin,gmax,ngbin,gmin,gmax);//for two-fold
  TH1F *hg2x=new TH1F("hg2x","total projection spectrum for hg2xy",ngbin,gmin,gmax);
  TH1F *hg2xp=new TH1F("hg2xp","net peaks of total projection spectrum of hg2xy",ngbin,gmin,gmax);
  TH1F *hg2xb;
  TH2F *hg2xyb=new TH2F("hg2xyb","background gg-matrix",ngbin,gmin,gmax,ngbin,gmin,gmax); 
  TH2F *hg2xyp=new TH2F("hg2xyp","background subtracted gg-matrix",ngbin,gmin,gmax,ngbin,gmin,gmax); 

  Int_t bins[4] = {ngbin, ngbin, ngbin};
  Double_t xmin[4] = {gmin, gmin, gmin};//must be Double_t, Float_t will casue error
  Double_t xmax[4] = {gmax, gmax, gmax};
  
  //for tree-fold 
  THnSparse *hg3xyz;//Sparse matrix.
  hg3xyz=new THnSparseF("hg3xyz", "g-g-g matrix", 3, bins, xmin, xmax);//for tree-fold
  TH2F *hg3xy=new TH2F("hg3xy","x-y projection of hg3xyz",ngbin,gmin,gmax,ngbin,gmin,gmax);
  TH1F *hg3x=new TH1F("hg3x","total projection spectrum for hg3xyz",ngbin,gmin,gmax);
  TH1F *hg3xp=new TH1F("hg3xp","net peaks of total projection spectrum of hg3xyz",ngbin,gmin,gmax);
  TH1F *hg3xb;
  
  //for four-fold
  //  THnSparse *hg4xyzw;//Sparse matrix.
  // hg4xyzw=new THnSparseF("hg4xyzw", "g-g-g-g matrix", 4, bins, xmin, xmax);//for four-fold
  //THnSparse *hg4xyz;//Sparse matrix.
  //hg4xyz=new THnSparseF("hg4xyz", "x-y-z projection of hg4xyzw", 3, bins, xmin, xmax);//  
  //TH2F *hg4xy=new TH2F("hg4xy","x-y projection of hg4xyzw",ngbin,gmin,gmax,ngbin,gmin,gmax);
  //TH1F *hg4x=new TH1F("hg4x","total projection spectrum for hg4xyzw",ngbin,gmin,gmax);
  //TH1F *hg4xp=new TH1F("hg4xp","net peaks of total projection spectrum of hg4xyzw",ngbin,gmin,gmax);
  //TH1F *hg4xb;

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
      //for inclusive
      if(ahit>0) {
	for(int i=0;i<ahit;i++)
	  hg1x->Fill(ae[i]);	
      }
      
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
	      bool bjk=abs(at[j]-at[k])>200;
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
      /*
    //for four-fold
      if(ahit>3) {
	for(int i=0; i<ahit; i++) {
	  for(int j=0; j<ahit; j++) {
	    for(int k=0; k<ahit; k++) {
	      for(int l=0; l<ahit; l++) {
		bool bij=abs(at[i]-at[j])>200;
		bool bik=abs(at[i]-at[k])>200;
		bool bil=abs(at[i]-at[l])>200;
		bool bjk=abs(at[j]-at[k])>200;
		bool bjl=abs(at[j]-at[l])>200;
		bool bkl=abs(at[k]-at[l])>200;	       
		if(bij || bik || bil || bjk || bjl || bkl) continue;
		if(i==j || i==k || i==l || j==k || j==l || k==l) continue;
		hg4x->Fill(ae[i]);
		hg4xy->Fill(ae[i],ae[j]);
		Double_t x3[3]={ae[i],ae[j],ae[k]};
		hg4xyz->Fill(x3);
		Double_t x4[4]={ae[i],ae[j],ae[k],ae[l]};
		hg4xyzw->Fill(x4);		
	      }
	    }
	  }
	}
      }
      */      
      if(jentry%500000==0) cout<<jentry<<endl;;
   }
   //"nosmoothing" option to avoid overestimate the background
   hg1xb=(TH1F*)sa->Background(hg1x,8,"nosmoothing");
   hg1xb->SetName("hg1xb");
   hg1xb->SetTitle("background of inclusive spectrum of hg1x");
   hg1xp->Add(hg1x,hg1xb,1,-1);   
   
   hg2xb=(TH1F*)sa->Background(hg2x,8,"nosmoothing");
   hg2xb->SetName("hg2xb");
   hg2xb->SetTitle("background of total projection spectrum of hg2xy");
   hg2xp->Add(hg2x,hg2xb,1,-1);
   //for two-fold
   Double_t T,Pi,Pj,pi,pj,Bij;
   T=hg2x->Integral();
   int N=hg2x->GetNbinsX();
   for(int i=1; i<=N; i++) {
     for(int j=1; j<=N; j++) {
       Pi=hg2x->GetBinContent(i);
       Pj=hg2x->GetBinContent(j);
       pi=hg2xp->GetBinContent(i);
       pj=hg2xp->GetBinContent(j);
       Bij=(Pi*Pj-pi*pj)/T;
       Double_t x=hg2x->GetBinCenter(i);
       Double_t y=hg2x->GetBinCenter(j);       
       hg2xyb->Fill(x,y,Bij);
     }
   }
   hg2xyp->Add(hg2xy,hg2xyb,1,-1);
   
   hg3xb=(TH1F*)sa->Background(hg3x,8,"nosmoothing same");
   hg3xb->SetName("hg3xb");
   hg3xb->SetTitle("background of total projection spectrum of hg3xyz");
   hg3xp->Add(hg3x,hg3xb,1,-1);

   hg4xb=(TH1F*)sa->Background(hg4x,8,"nosmoothing same");
   hg4xb->SetName("hg4xb");
   hg4xb->SetTitle("background of total projection spectrum of hg4xyzw");
   hg4xp->Add(hg3x,hg3xb,1,-1);

   fout->cd();

   hg1x->Write();
   hg1xb->Write();
   
   hg2xy->Write();
   hg2xyb->Write();
   hg2xyp->Write();
   hg2x->Write();
   hg2xb->Write();
   hg2xp->Write();

   hg3xyz->Write();
   hg3xy->Write();
   hg3x->Write();
   hg3xb->Write();
   hg3xp->Write();

   hg4xyzw->Write();
   hg4xyz->Write();
   hg4xy->Write();
   hg4x->Write();
   hg4xb->Write();
   hg4xp->Write();

   fout->Close();
}
