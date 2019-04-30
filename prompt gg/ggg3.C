TH1F *hgb,*hg,*hga;
Double_t getv(TH2F *h,int i, int j)
{
  return h->GetBinContent(i,j);
}

Double_t getv(TH1F *h,int i)
{
  return h->GetBinContent(i);
}

void ggg3(Double_t gy, Double_t gz)
{
  
  TFile *fg=new TFile("gamma.root");
  THnSparseF *hg3xyz=(THnSparseF*)fg->Get("hg3xyz");
  TH2F *hg3xy=(TH2F*)fg->Get("hg3xy");//Mij
  TH1F *hg3x=(TH1F*)fg->Get("hg3x");//Pi
  TH1F *hg3xb=(TH1F*)fg->Get("hg3xb");//bi
  hgb=(TH1F*)hg3x->Clone("hgb");
  hgb->Reset();
	
  hga=(TH1F*)hg3x->Clone("hga");
  hga->SetName(Form("g_%d_%d",int(gy),int(gz)));    
  hga->SetTitle(Form("gated by %.1f and %.1f",gy,gz)); 
  hga->Reset(); 

  Double_t gw=3;
  int iy1=hg3xyz->GetAxis(1)->FindBin(gy-gw);
  int iy2=hg3xyz->GetAxis(1)->FindBin(gy+gw);
  int iz1=hg3xyz->GetAxis(2)->FindBin(gz-gw);
  int iz2=hg3xyz->GetAxis(2)->FindBin(gz+gw); 
  hg3xyz->GetAxis(1)->SetRange(iy1,iy2);
  hg3xyz->GetAxis(2)->SetRange(iz1,iz2);
  hg=(TH1F*)hg3xyz->Projection(0);
   
  Double_t T,Mij,Mjk,Mik,Pi,Pj,Pk,bi,bj,bk,Bijk;
  T=hg3x->Integral();
  int N=hg3x->GetNbinsX();
  for(int i=1; i<=N; i++) {
    for(int j=iy1; j<=iy2; j++) {
      for(int k=iz1; k<=iz2; k++) {
	Mij=getv(hg3xy,i,j);
	Mik=getv(hg3xy,i,k);
	Mjk=getv(hg3xy,j,k);
	Pi=getv(hg3x,i);
	Pj=getv(hg3x,j);
	Pk=getv(hg3x,k);	
	bi=getv(hg3xb,i);
	bj=getv(hg3xb,j);
	bk=getv(hg3xb,k);	
	Bijk=(Mij*bk+Mik*bj+Mjk*bi)/T
	  +(-Pi*bj*bk-bi*Pj*bk-bi*bj*Pk+bi*bj*bk)/T/T;
	hgb->Fill(hgb->GetBinCenter(i),Bijk);
      }
    }
  }
  hgb->Sumw2(0);
  hga->Add(hg,hgb,1,-1);
  hga->Draw();
  

}
