TH1F *hg,*hgb,*hga;
void gg(Double_t gy)
{
  
  TFile *fg=new TFile("gamma.root");
  TH2F *hg2xy=(TH2F*)fg->Get("hg2xy");
  TH1F *hg2x=(TH1F*)fg->Get("hg2x");//Pi
  TH1F *hg2xb=(TH1F*)fg->Get("hg2xb");//bi
  TH1F *hg2xp=(TH1F*)fg->Get("hg2xp");//pi  
  hgb=(TH1F*)hg2x->Clone("hgb");
  hgb->Reset();

  hga=(TH1F*)hg2x->Clone("hga");
  hga->SetName(Form("g_%d",int(gy)));    
  hga->SetTitle(Form("gated by %.1f",gy)); 
  hga->Reset();

  Double_t gw=3;
  int iy1=hg2xy->GetYaxis()->FindBin(gy-gw);
  int iy2=hg2xy->GetYaxis()->FindBin(gy+gw);

  hg=(TH1F*)hg2xy->ProjectionY("hg",iy1,iy2);
  
  Double_t T,Pi,Pj,pi,pj,Bij;
  T=hg2x->Integral();
  cout<<T<<endl;
  int N=hg2x->GetNbinsX();
  for(int i=1; i<=N; i++) {
    for(int j=iy1; j<=iy2; j++) {
	Pi=hg2x->GetBinContent(i);
	Pj=hg2x->GetBinContent(j);
	pi=hg2xp->GetBinContent(i);
	pj=hg2xp->GetBinContent(j);
	Bij=(Pi*Pj-pi*pj)/T;
	Double_t x=hg2x->GetBinCenter(i);
	hgb->Fill(x,Bij);
      }
    }
  hgb->Sumw2(0);
  hga->Add(hg,hgb,1,-1);
  hga->Draw();
  

}
