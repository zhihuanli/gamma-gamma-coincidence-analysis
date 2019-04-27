struct parameters
{
  int xmin;
  int xmax;//range for hist
  int npeaks;//number of peaks for TSpectrum
  int dge1;//gate width:ge+dge1 - ge+dge2
  int dge2;
  bool peak_background;//true: all negative counts will be set to 0 
};
parameters st;
TList *tall;

TString fname="d46_123ggs.root";//filename;
//TString fname="46_123gg300s.root";//filename;

void gg();
void setxrange(int xmin1=0, int xmax1=2000){ st.xmin=xmin1; st.xmax=xmax1;};
void setnpeaks(int npeaks1=30) {st.npeaks=npeaks1;};
void setpeakwidth(double dgea=-3,double dgeb=3) {st.dge1=dgea; st.dge2=dgeb;};
void setpeakbackground(bool pb=false) {st.peak_background=pb;};
void newcanvas(int ncy=1);
void tpjm();
void tpj(int icy=1);
void gaem();
void gae(int icy=1);
TString g(double ge,int icy1=1);//return name of histogram;
TString gw(double ge1,double ge2, int icy1=1);
TString gand(double ge1,double ge2,double ge3=0,double ge4=0,
	  double ge5=0,double ge6=0);
TString gadd(double ge1,double ge2,double ge3=0,double ge4=0,
	  double ge5=0,double ge6=0);
TString gsub(double ge1,double ge2,double ge3=0,double ge4=0,
	     double ge5=0,double ge6=0);
void gm(double ge1,double ge2=0,double ge3=0,double ge4=0,
	double ge5=0,double ge6=0);//draw multi peaks in a canvas , up to six peaks.
void gm(TH1* h1, TH1 *h2=NULL, TH1 *h3=NULL, TH1 *h4=NULL,
	TH1 *h5=NULL, TH1 *h6=NULL);//show multi-gated peaks 
TString peaks(TH1 *h, Double_t thres=0.05);
void show(){tall->ls();};//show name of all histograms


TFile *f;
TH1D *xe,*ye,*aae;
TH2I *ggm;
TCanvas *ca[1000];
int ic=-1;//canvas id
int icy=1;
int ncy=1;//number of windows in y axia
int ih=0;
int ihw=0;
int ihand=0;
int ihadd=0;
int ihsub=0;

void gg()
{
  if(f==NULL) {
    f=new TFile(fname);
    cout<<"load ROOT file: "<<fname<<endl;
  }
  xe=(TH1D*)f->Get("TpjPeak");
  xe->SetTitle(xe->GetName());
  aae=(TH1D*)f->Get("ae");
  aae->SetTitle(aae->GetName());
  ggm=(TH2I*)f->Get("ggmat");
  ggm->SetTitle(ggm->GetName());
  setxrange(0,2000);
  setnpeaks(30);
  setpeakwidth(-3,3);
  setpeakbackground(0);//
  if(tall==NULL) tall=new TList();
  tpjm();
  if(aae!=NULL) gaem();
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
  TH1D *h=(TH1D*)xe->Clone(sname);
  h->SetTitle(xe->GetTitle());
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
void gae(int icy)
{
  if(icy>ncy) icy=ncy;
  ca[ic]->cd(icy);
  TString sname=Form("ae%i",ih++);
  TH1D *h=(TH1D*)aae->Clone(sname);
  h->SetTitle(aae->GetTitle());
  peaks(h);
}
//gated on x axis
TString g(double ge, int icy1)
{
  if(icy1>ncy) icy=ncy;
  else icy=icy1;
  ca[ic]->cd(icy);
  TString sha=Form("gated on ge=%.1f",ge);
  double gea=ggm->GetXaxis()->FindBin(ge+st.dge1);
  double geb=ggm->GetXaxis()->FindBin(ge+st.dge2); 
  TH1D *ha=(TH1D*)ggm->ProjectionX(Form("g%d_%i",int(ge),ih++),gea,geb);
  ha->SetTitle(sha);
  tall->Add(ha);
  return peaks(ha);
  
}

//gated on x axis
TString gw(double ge1,double ge2, int icy1)
{
  if(icy1>ncy) icy=ncy;
  else icy=icy1;
  ca[ic]->cd(icy);
  TString sha=Form("gated on ge=%.1f-%.1f",ge1,ge2);
  double gea=ggm->GetXaxis()->FindBin(ge1);
  double geb=ggm->GetXaxis()->FindBin(ge2); 
  TH1D *ha=(TH1D*)ggm->ProjectionX(Form("gw%da%i",int(ge1),ihw++),gea,geb);
  ha->SetTitle(sha);
  tall->Add(ha);
  return peaks(ha);
  
}

//gated on x axis
TString gand(double ge1, double ge2, double ge3,
	   double ge4,double ge5,double ge6)
{

  int npad=0;
  TH1D *ha[6];
  double ge[6]={ge1,ge2,ge3,ge4,ge5,ge6};
  for(int i=0;i<6;i++) 
    if(ge[i]>1) npad++;
  newcanvas(npad+1);
  TString sha="And gate of";
  for(int i=0;i<npad;i++) {
    TString hname=g(ge[i],i+1);//return name of histogram.
    ha[i]=(TH1D*)gROOT->FindObject(hname);
    sha=Form("%s %.1f",sha.Data(),ge[i]);
  }
  TString sname=Form("gand%i",ihand++);
  TH1D *hand=(TH1D*)ha[0]->Clone(sname);
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

//gated on x axis
TString gadd(double ge1, double ge2, double ge3,
	   double ge4,double ge5,double ge6)
{

  int npad=0;
  TH1D *ha[6];
  double ge[6]={ge1,ge2,ge3,ge4,ge5,ge6};
  for(int i=0;i<6;i++) 
    if(ge[i]>1) npad++;
  newcanvas(npad+1);
  TString sha="Add gate of";
  for(int i=0;i<npad;i++) {
    TString hname=g(ge[i],i+1);//return name of histogram.
    ha[i]=(TH1D*)gROOT->FindObject(hname);
    sha=Form("%s %.1f",sha.Data(),ge[i]);
  }
  TString sname=Form("gadd%i",ihadd++);
  TH1D *hadd=(TH1D*)ha[0]->Clone(sname);
  hadd->SetTitle(sha);
  for(int i=1;i<npad;i++) hadd->Add(hadd,ha[i]);
  tall->Add(hadd);
  ca[ic]->cd(npad+1);
  icy=npad+1;
  return peaks(hadd);
}

TString gsub(double ge1, double ge2, double ge3,
	   double ge4,double ge5,double ge6)
{

  int npad=0;
  TH1D *ha[6];
  double ge[6]={ge1,ge2,ge3,ge4,ge5,ge6};
  for(int i=0;i<6;i++) 
    if(ge[i]>1) npad++;
  newcanvas(npad+1);
  TString sha="sub gate of";
  for(int i=0;i<npad;i++) {
    TString hname=g(ge[i],i+1);//return name of histogram.
    ha[i]=(TH1D*)gROOT->FindObject(hname);
    sha=Form("%s %.1f",sha.Data(),ge[i]);
  }
  TString sname=Form("gsub%i",ihsub++);
  TH1D *hsub=(TH1D*)ha[0]->Clone(sname);
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
      TH1D *hb=(TH1D*)s->Background(h,5,"same");
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
   TString s1=ss.str();
   TLatex *tex=new TLatex(xpeaks[j],ypeaks[j],s1);
   tex->SetTextFont(13);
   tex->SetTextSize(13);
   tex->SetTextAlign(12);
   tex->SetTextAngle(90);
   tex->SetTextColor(kRed);
   tex->Draw();
  }
  return h->GetName();
}
