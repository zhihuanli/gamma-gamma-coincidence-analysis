//gamma-gamma coincidence analysis 
//by Zhihuan Li, Peking University
//Based on gf3.c of RADWARE
//last update Feb. 2020
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

TString fname="gg46_123_200ms.root";//filename;

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

  
TFile *f;

//one-fold
TH1F *hg1x;

//two-fold
TH1F *hg2xp;
TH2F *hg2xyp;

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
