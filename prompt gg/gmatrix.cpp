void gmatrix()
{
  //input root file;
  TFile *fin=new TFile("/data/d2/zhli/hepfarm/ur12ana/test/srootall/decay46_123_all.root");
  TTree *tree=(TTree*)fin->Get("tree");
  Int_t           ahit;
  Double_t        ae[100];  
  Double_t        at[100];  
  Int_t           aid[100];   
  Double_t        decaytime;

  tree->SetBranchAddress("ahit",&ahit);
  tree->SetBranchAddress("ae",&ae);
  tree->SetBranchAddress("at",&at);
  tree->SetBranchAddress("aid",&aid);
  tree->SetBranchAddress("decaytime",&decaytime);
  //output file
  TFile *fout=new TFile("gg46_123_200ms.root","RECREATE");//decaytime<200ms;

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

  TSpectrum *sa=new TSpectrum(200);
  
   Long64_t nentries = tree->GetEntriesFast();
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      tree->GetEntry(jentry); 
      if(ahit>0) {
	for(int i=0;i<ahit;i++)
	  hg1x->Fill(ae[i]);	
      }
      
      //two-fold gamma-gamma matrix
      if(ahit>1) {
	for(int i=0; i<ahit; i++) {
	  for(int j=0; j<ahit; j++) {
	    if(i==j)  continue;
	    bool btcut=abs(at[i]-at[j])<200; //prompt coincidence time window
	    bool bdtcut=decaytime<200;//decaytime cut	    
	    if(btcut && bdtcut) {
	    	hg2x->Fill(ae[i]);
	    	hg2xy->Fill(ae[i],ae[j]);
	    }
	  }
	}
      }
      if(jentry%500000==0) cout<<jentry<<endl;;
   }
   //"nosmoothing" option to avoid overestimation of the background
   hg1xb=(TH1F*)sa->Background(hg1x,8,"nosmoothing")
   hg1xb->SetName("hg1xb");
   hg1xb->SetTitle("background of inclusive spectrum of hg1x");
   hg1xp->Add(hg1x,hg1xb,1,-1);   
   
   hg2xb=(TH1F*)sa->Background(hg2x,8,"nosmoothing");
   hg2xb->SetName("hg2xb");
   hg2xb->SetTitle("background of total projection spectrum of hg2xy");
   hg2xp->Add(hg2x,hg2xb,1,-1);
	
   //make background matrix - RADWARE approch
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

   fout->cd();
   hg1x->Write();
   hg1xb->Write();
   
   hg2xy->Write();
   hg2xyb->Write();
   hg2xyp->Write();
   hg2x->Write();
   hg2xb->Write();
   hg2xp->Write();
   fout->Close();
}
