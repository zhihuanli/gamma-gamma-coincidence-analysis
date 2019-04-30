TH1F *hgb,*hg,*hga;
Double_t getv(THnSparseF* h, int i, int j, int k )
{
  int x[3]={i,j,k};
  return h->GetBinContent(x);
}

Double_t getv(TH2F *h,int i, int j)
{
  return h->GetBinContent(i,j);
}

Double_t getv(TH1F *h,int i)
{
  return h->GetBinContent(i);
}

void gggg4(Double_t gy, Double_t gz, Double_t gw)
{
  
  TFile *fg=new TFile("gamma.root");
  THnSparseF *hg4xyzw=(THnSparseF*)fg->Get("hg4xyzw");
  THnSparseF *hg4xyz=(THnSparseF*)fg->Get("hg4xyz");
  TH2F *hg4xy=(TH2F*)fg->Get("hg4xy");//Mij
  TH1F *hg4x=(TH1F*)fg->Get("hg4x");//Pi
  TH1F *hg4xb=(TH1F*)fg->Get("hg4xb");//bi
  hgb=(TH1F*)hg4x->Clone("hgb");
  hgb->Reset();
	
  hga=(TH1F*)hg4x->Clone("hga");
  hga->SetName(Form("g_%d_%d_%d",int(gy),int(gz),int(gw)));    
  hga->SetTitle(Form("gated by %.1f and %.1f and %.1f",gy,gz,gw)); 
  hga->Reset(); 

  Double_t ghw=3;
  int iy1=hg4xyzw->GetAxis(1)->FindBin(gy-ghw);
  int iy2=hg4xyzw->GetAxis(1)->FindBin(gy+ghw);
  int iz1=hg4xyzw->GetAxis(2)->FindBin(gz-ghw);
  int iz2=hg4xyzw->GetAxis(2)->FindBin(gz+ghw);
  int iw1=hg4xyzw->GetAxis(3)->FindBin(gw-ghw);
  int iw2=hg4xyzw->GetAxis(3)->FindBin(gw+ghw);
  
  hg4xyzw->GetAxis(1)->SetRange(iy1,iy2);
  hg4xyzw->GetAxis(2)->SetRange(iz1,iz2);
  hg4xyzw->GetAxis(3)->SetRange(iw1,iw2);  
  hg=(TH1F*)hg4xyzw->Projection(0);
   
  Double_t T,Cijk,Cijl,Cikl,Cjlk,Mij,Mik,Mjk,Mil,Mjl,Mkl;
  Double_t Pi,Pj,Pk,Pl,bi,bj,bk,bl,Bijkl;
  T=hg4x->Integral();
  int N=hg4x->GetNbinsX();
  for(int i=1; i<=N; i++) {
    for(int j=iy1; j<=iy2; j++) {
      for(int k=iz1; k<=iz2; k++) {
	for(int l=iw1; l<=iw2; l++) {
	  Cijk=(hg4xyz,i,j,k);
	  Cijl=(hg4xyz,i,j,l);
	  Cikl=(hg4xyz,i,k,l);
	  Cjlk=(hg4xyz,j,l,k);
	  
	  Mij=getv(hg4xy,i,j);
	  Mik=getv(hg4xy,i,k);
	  Mjk=getv(hg4xy,j,k);
	  Mil=getv(hg4xy,i,l);
	  Mjl=getv(hg4xy,j,l);
	  Mkl=getv(hg4xy,k,l);
	  
	  Pi=getv(hg4x,i);
	  Pj=getv(hg4x,j);
	  Pk=getv(hg4x,k);
	  Pl=getv(hg4x,l);
	  
	  bi=getv(hg4xb,i);
	  bj=getv(hg4xb,j);
	  bk=getv(hg4xb,k);
	  bl=getv(hg4xb,l);
	  Bijkl=(Cijk*bl+Cijl*bk+Cikl*bj+Cjlk*bi)/T
	    +(-Mij*bk*bl-Mik*bj*bl-Mjk*bi*bl-Mil*bj*bk-Mjl*bi*bk-Mkl*bi*bj)/T/T
	    +(Pi*bj*bk*bl+bi*Pj*bk*bl+bi*bj*Pk*bl+bi*bj*bk*Pl-bi*bj*bk*bl)/T/T/T;
	  hgb->Fill(hgb->GetBinCenter(i),Bijkl);
	}
      }
    }
  }
  hgb->Sumw2(0);
  hga->Add(hg,hgb,1,-1);
  hga->Draw();
  

}
