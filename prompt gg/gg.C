struct parameters
{
  int xmin;
  int xmax;    //range for hist
  int npeaks;  //number of peaks for TSpectrum
  int dge1;    //gate width:ge+dge1 - ge+dge2
  int dge2;
  bool peak_background;//true: all negative counts will be set to 0 
};
parameters st;
TList *tall;

TString fname="gamma.root";//filename;

void gg();
void setxrange(int xmin1=0, int xmax1=2000){ st.xmin=xmin1; st.xmax=xmax1;};
void setnpeaks(int npeaks1=30) {st.npeaks=npeaks1;};
void setpeakwidth(double dgea=-2,double dgeb=2) {st.dge1=dgea; st.dge2=dgeb;};
void setpeakbackground(bool pb=false) {st.peak_background=pb;};
void newcanvas(int ncy=1);

void tpjm();
void tpj(int icy=1);
void gaem();
void gae(int icy=1);

//two-fold gated
TString g(double ge=0,int icy1=1);
//three-fold gated
TString g2(double ge1=0,double ge2=0, int icy1=1);
//four-fold gated --experimental.
//TString g3(double ge1=0,double ge2=0, double ge3=0, int icy1=1);
//two-fold gated with a specified gamma peak range
TString gw(double ge1=0,double ge2=0, int icy1=1);
//and
TString gand(double ge1=0,double ge2=0,double ge3=0,double ge4=0, double ge5=0,double ge6=0);
//add 
TString gadd(double ge1=0,double ge2=0,double ge3=0,double ge4=0, double ge5=0,double ge6=0);
//sub
TString gsub(double ge1=0,double ge2=0,double ge3=0,double ge4=0, double ge5=0,double ge6=0);
//multiple gated-spectra up to six peaks - draw new spectra
void gm(double ge1=0,double ge2=0,double ge3=0,double ge4=0, double ge5=0,double ge6=0);
//multiple gated-spectra up to six peaks - draw existing histograms
void gm(TH1* h1, TH1 *h2=NULL, TH1 *h3=NULL, TH1 *h4=NULL,TH1 *h5=NULL, TH1 *h6=NULL);
//mark peak positions
TString peaks(TH1 *h, Double_t thres=0.05);
//show all existing gated histograms
void show(){tall->ls();};//show name of all histograms

Double_t getv(THnSparseF* h, int i, int j, int k );
Double_t getv(TH2F* h, int i, int j);
Double_t getv(TH1F* h, int i);
  
TFile *f;

//one-fold
TH1F *hg1x;

//two-fold
TH1F *hg2xp;
TH2F *hg2xyp;

//three-fold
THnSparseF *hg3xyz;
TH2F *hg3xy;
TH1F *hg3x, *hg3xb;

//four-fold
//THnSparseF *hg4xyzw;
//THnSparseF *hg4xyz;
//TH2F *hg4xy;
//TH1F *hg4x, *hg4xb;

TCanvas *ca[1000];
int ic=-1;//canvas id
int icy=1;
int ncy=1;//number of windows in y axia
int ih=0;
int ihw=0;
int ihand=0;
int ihadd=0;
int ihsub=0;
int ihtrip=0;
int ihquad=0;

void gg()
{
  if(f==NULL) {
    f=new TFile(fname);
    cout<<"load ROOT file: "<<fname<<endl;
  }
  
  //one-fold
  hg1x=(TH1F*)f->Get("hg1x");
  
  //two-fold
  hg2xp=(TH1F*)f->Get("hg2xp");
  hg2xyp=(TH2F*)f->Get("hg2xyp");
  
  //three-fold
  hg3xyz=(THnSparseF*)f->Get("hg3xyz");
  hg3xy=(TH2F*)f->Get("hg3xy");
  hg3x=(TH1F*)f->Get("hg3x");
  hg3xb=(TH1F*)f->Get("hg3xb");
  
  //four-fold
  // hg4xyzw=(THnSparseF*)f->Get("hg4xyzw");
  //hg4xyz=(THnSparseF*)f->Get("hg4xyz");
  //hg4xy=(TH2F*)f->Get("hg4xy");//Mij
  //hg4x=(TH1F*)f->Get("hg4x");//Pi
  //hg4xb=(TH1F*)f->Get("hg4xb");//bi

  setxrange(0,2000);
  setnpeaks(30);
  setpeakwidth(-3,3);
  setpeakbackground(0);//
  if(tall==NULL) tall=new TList();
  tpjm();
  // if(aae!=NULL) gaem();
}

void tpjm()
{
  newcanvas(2);
  setxrange(0,1000);
  tpj(1);
  setxrange(750,2000);
  tpj(2);
  setxrange();
  
}
void tpj(int icy)
{
  if(icy>ncy) icy=ncy;
  ca[ic]->cd(icy);
  TString sname=Form("gtpj%i",ih++);
  TH1F *h=(TH1F*)hg2xp->Clone(sname);
  peaks(h);
}

void gaem()
{
  newcanvas(2);
  setxrange(0,1000);
  gae(1);
  setxrange(1000,2000);
  gae(2);
  setxrange();
  
}
void gae(int icy1)
{
  if(icy1>ncy) icy=ncy;
  ca[ic]->cd(icy);
  TString sname=Form("ae%i",ih++);
  TH1F *h=(TH1F*)hg1x->Clone(sname);
  peaks(h);
}
//two fold - gated on x axis
TString g(double ge, int icy1)
{
  if(ge==0) {
    cout<<"wrong parameters!"<<endl;
    return "error";
  }
  if(icy1>ncy) icy=ncy;
  else icy=icy1;
  ca[ic]->cd(icy);
  TString sha=Form("gated on %.1f keV",ge);
  double gea=hg2xyp->GetXaxis()->FindBin(ge+st.dge1);
  double geb=hg2xyp->GetXaxis()->FindBin(ge+st.dge2); 
  TH1F *ha=(TH1F*)hg2xyp->ProjectionX(Form("g%d_%i",int(ge),ih++),gea,geb);
  ha->SetTitle(sha);
  tall->Add(ha);
  return peaks(ha);
  
}

//three-fold gated on x and y axis
TString g2(double ge1, double ge2, int icy1)
{
  if(ge1==0 || ge2==0) {
    cout<<"wrong parameters!"<<endl;
    return "error";
  }
  if(icy1>ncy) icy=ncy;
  else icy=icy1;
  ca[ic]->cd(icy);
  
  int iy1=hg3xyz->GetAxis(1)->FindBin(ge1+st.dge1);
  int iy2=hg3xyz->GetAxis(1)->FindBin(ge1+st.dge2);
  int iz1=hg3xyz->GetAxis(2)->FindBin(ge2+st.dge1);
  int iz2=hg3xyz->GetAxis(2)->FindBin(ge2+st.dge2); 
  hg3xyz->GetAxis(1)->SetRange(iy1,iy2);
  hg3xyz->GetAxis(2)->SetRange(iz1,iz2);
  TH1F *ha=(TH1F*)hg3xyz->Projection(0);

  TH1F *hgb=(TH1F*)hg3x->Clone("hgb");
  hgb->Reset();
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
  ha->Add(ha,hgb,1,-1);
  ha->SetTitle(Form("two-fold gates by %.1f_%.1f keV",ge1,ge2));
  ha->SetName(Form("gtrip%i",ihtrip++));
  tall->Add(ha);
  return peaks(ha);
}
/*
//four-fold gated on x and y axis -- experimental
TString g3(double ge1, double ge2, double ge3,int icy1)
{
  if(ge1==0 || ge2==0 || ge3==0) {
    cout<<"wrong parameters!"<<endl;
    return "error";
  }
  if(icy1>ncy) icy=ncy;
  else icy=icy1;
  ca[ic]->cd(icy);
  
  int iy1=hg4xyzw->GetAxis(1)->FindBin(ge1+st.dge1);
  int iy2=hg4xyzw->GetAxis(1)->FindBin(ge1+st.dge2);
  int iz1=hg4xyzw->GetAxis(2)->FindBin(ge2+st.dge1);
  int iz2=hg4xyzw->GetAxis(2)->FindBin(ge2+st.dge2);
  int iw1=hg4xyzw->GetAxis(3)->FindBin(ge3+st.dge1);
  int iw2=hg4xyzw->GetAxis(3)->FindBin(ge3+st.dge2);
  hg4xyzw->GetAxis(1)->SetRange(iy1,iy2);
  hg4xyzw->GetAxis(2)->SetRange(iz1,iz2);
  hg4xyzw->GetAxis(3)->SetRange(iw1,iw2);  
  TH1F *ha=(TH1F*)hg4xyzw->Projection(0);
  TH1F *hgb=(TH1F*)hg4x->Clone("hgb");
  hgb->Reset();
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
  ha->Add(ha,hgb,1,-1);
  ha->SetTitle(Form("three-fold gates by %.1f_%.1f_%.1f ",ge1,ge2,ge3));
  ha->SetName(Form("gquad%i",ihquad++));
  tall->Add(ha);
  return peaks(ha);
}

*/
TString gw(double ge1,double ge2, int icy1)
{
  if(ge1==0 || ge2==0) {
    cout<<"wrong parameters!"<<endl;
    return "error";
  }
  if(icy1>ncy) icy=ncy;
  else icy=icy1;
  ca[ic]->cd(icy);
  TString sha=Form("gated on %.1f-%.1f keV",ge1,ge2);
  double gea=hg2xyp->GetXaxis()->FindBin(ge1);
  double geb=hg2xyp->GetXaxis()->FindBin(ge2); 
  TH1F *ha=(TH1F*)hg2xyp->ProjectionX(Form("gw%da%i",int(ge1),ihw++),gea,geb);
  ha->SetTitle(sha);
  tall->Add(ha);
  return peaks(ha);
  
}


TString gand(double ge1, double ge2, double ge3, double ge4,double ge5,double ge6)
{
  if(ge1==0 || ge2==0) {
    cout<<"wrong parameters!"<<endl;
    return "error";
  }
  int npad=0;
  TH1F *ha[6];
  double ge[6]={ge1,ge2,ge3,ge4,ge5,ge6};
  for(int i=0;i<6;i++) 
    if(ge[i]>1) npad++;
  newcanvas(npad+1);
  TString sha="And gate of";
  for(int i=0;i<npad;i++) {
    TString hname=g(ge[i],i+1);//return name of histogram.
    ha[i]=(TH1F*)gROOT->FindObject(hname);
    sha=Form("%s %.1f",sha.Data(),ge[i]);
  }
  sha=Form("%s keV",sha.Data());
  TString sname=Form("gand%i",ihand++);
  TH1F *hand=(TH1F*)ha[0]->Clone(sname);
  hand->SetTitle(sha);
  hand->Reset("M");//!!! reset maximum and minimum;
  for(int i=1;i<=ha[0]->GetNbinsX();i++){
    double ma=ha[0]->GetBinContent(i);
    for(int j=1;j<npad;j++) {
      double mb=ha[j]->GetBinContent(i);
      ma=TMath::Min(ma,mb);
    }
    hand->Fill(hand->GetBinCenter(i),ma);
  }
  tall->Add(hand);
  ca[ic]->cd(npad+1);
  icy=npad+1;
  return peaks(hand);
}


TString gadd(double ge1, double ge2, double ge3, double ge4,double ge5,double ge6)
{
  if(ge1==0 || ge2==0) {
    cout<<"wrong parameters!"<<endl;
    return "error";
  }
  int npad=0;
  TH1F *ha[6];
  double ge[6]={ge1,ge2,ge3,ge4,ge5,ge6};
  for(int i=0;i<6;i++) 
    if(ge[i]>1) npad++;
  newcanvas(npad+1);
  TString sha="Add gate of";
  for(int i=0;i<npad;i++) {
    TString hname=g(ge[i],i+1);//return name of histogram.
    ha[i]=(TH1F*)gROOT->FindObject(hname);
    sha=Form("%s %.1f",sha.Data(),ge[i]);
  }
  sha=Form("%s keV",sha.Data());  
  TString sname=Form("gadd%i",ihadd++);
  TH1F *hadd=(TH1F*)ha[0]->Clone(sname);
  hadd->SetTitle(sha);
  for(int i=1;i<npad;i++) hadd->Add(hadd,ha[i]);
  tall->Add(hadd);
  ca[ic]->cd(npad+1);
  icy=npad+1;
  return peaks(hadd);
}

TString gsub(double ge1, double ge2, double ge3, double ge4,double ge5,double ge6)
{

  if(ge1==0 || ge2==0) {
    cout<<"wrong parameters!"<<endl;
    return "error";
  }
  int npad=0;
  TH1F *ha[6];
  double ge[6]={ge1,ge2,ge3,ge4,ge5,ge6};
  for(int i=0;i<6;i++) 
    if(ge[i]>1) npad++;
  newcanvas(npad+1);
  TString sha="sub gate of";
  for(int i=0;i<npad;i++) {
    TString hname=g(ge[i],i+1);//return name of histogram.
    ha[i]=(TH1F*)gROOT->FindObject(hname);
    sha=Form("%s %.1f",sha.Data(),ge[i]);
  }
  sha=Form("%s keV",sha.Data());
  TString sname=Form("gsub%i",ihsub++);
  TH1F *hsub=(TH1F*)ha[0]->Clone(sname);
  hsub->SetTitle(sha);
  for(int i=1;i<npad;i++) hsub->Add(hsub,ha[i],1,-1);
  tall->Add(hsub);
  ca[ic]->cd(npad+1);
  icy=npad+1;
  return peaks(hsub);
}

void gm(double ge1,double ge2,double ge3,double ge4, double ge5,double ge6)
{
  int npad=0;
  double ge[6]={ge1,ge2,ge3,ge4,ge5,ge6};
  for(int i=0;i<6;i++) 
    if(ge[i]>1) npad++;
  newcanvas(npad);
  for(int i=0;i<npad;i++)
    g(ge[i],i+1);
}

void gm(TH1* h1, TH1 *h2, TH1 *h3, TH1 *h4, TH1 *h5, TH1 *h6)
{
  int npad=0;
  TH1 *hg[6]={h1,h2,h3,h4,h5,h6};
  for(int i=0;i<6;i++) 
    if(hg[i]!=NULL) npad++;
  newcanvas(npad);
  for(int i=0;i<npad;i++) {
    ca[ic]->cd(i+1);
    peaks(hg[i]);
  }
}
void newcanvas(int ncy1=1)
{
  ic++;
  ncy=ncy1;
  double w=800;
  double h=800;
  if(ncy1==1)h=400;
  ca[ic]=new TCanvas(Form("ca%i",ic),Form("canvas%i",ic),w,h);
  ca[ic]->Divide(1,ncy);
  for(int i=1;i<=ncy;i++) {
    ca[ic]->GetPad(i)->SetBottomMargin(0.05);
    ca[ic]->GetPad(i)->SetTopMargin(0.005);
    ca[ic]->GetPad(i)->SetLeftMargin(0.05);
    ca[ic]->GetPad(i)->SetRightMargin(0.05);
  }

}

TString peaks(TH1 *h, Double_t thres)
{
  double x0=h->GetBinLowEdge(h->GetNbinsX());
  double x1=h->GetBinLowEdge(0);
  if(st.xmin<x1) st.xmin=x1;
  if(st.xmax>x0) st.xmax=x0;
  h->SetAxisRange(st.xmin,st.xmax,"X");
  double ymin=h->GetBinContent(h->GetMinimumBin());
  double ymax=h->GetBinContent(h->GetMaximumBin())*1.3;
  if(ymin<0) ymin=0.01;
  cout<<ic<<" "<<icy<<" "<<h->GetName()<<"  ["<<h->GetTitle()<<"]"<<endl; 
  h->SetLineColor(kBlue);
  h->SetFillColor(kCyan);
  TSpectrum *s=new TSpectrum(500);
  if(st.peak_background) {
    for(int i=1;i<=h->GetNbinsX();i++) //set bins with negative counts to zero
       if(h->GetBinContent(i)<0)  h->SetBinContent(i,0);
      TH1F *hb=(TH1F*)s->Background(h,5,"same");
      h->Add(h,hb,1,-1);
  }
    h->Sumw2(0);
  h->SetAxisRange(ymin,ymax,"Y");
  h->SetStats(0);
  Int_t nfound=100;
  Int_t nloop=0;
   while(nloop<50){
    nfound=s->Search(h,2,"",thres);
    if(nfound>st.npeaks) thres += 0.005;
    else thres -= 0.005;
    if(thres<0 || abs(nfound-st.npeaks)<3 ) break;
    nloop++;
    }
  TPolyMarker *pm=(TPolyMarker *)
                      h->GetListOfFunctions()->FindObject("TPolyMarker");
  pm->SetMarkerStyle(32);
  pm->SetMarkerColor(kGreen);
  pm->SetMarkerSize(0.4);
  Double_t *xpeaks=s->GetPositionX();
  Double_t *ypeaks=s->GetPositionY();
  for(int j=0;j<nfound;j++) {
   stringstream ss;
   ss<<xpeaks[j];
   if(ypeaks[j]<3) continue;
   TString s1=ss.str();
   TLatex *tex=new TLatex(xpeaks[j],ypeaks[j],s1);
   tex->SetTextFont(13);
   tex->SetTextSize(13);
   tex->SetTextAlign(12);
   tex->SetTextAngle(90);
   tex->SetTextColor(kRed);
   tex->Draw();
  }
  ca[ic]->Draw();
  return h->GetName();
}
 
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
