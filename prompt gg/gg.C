int xmin;
int xmax;//range for hist
int npeaks;//number of peaks for TSpectrum
int dge1;//gate width:ge+dge1 - ge+dge2
int dge2;
TString fname="xia.root";//filename;

void gg();
void setxrange(int xmin1=0, int xmax1=3072){  xmin=xmin1; xmax=xmax1;};
void setnpeaks(int npeaks1=30) {npeaks=npeaks1;};
void setpeakwidth(double dgea=-3,double dgeb=3) {dge1=dgea;dge2=dgeb;};
void newcanvas(int ncy=1);
void tpjm();
void tpj(int icy=1);
void g(double ge,int icy=1);
void gm(double ge1,double ge2=0,double ge3=0,double ge4=0,
	double ge5=0,double ge6=0);//draw multi peaks in a canvas , up to six peaks.
void peaks(TH1 *h, Double_t thres=0.05);


TFile *f;
TH1D *xe,*ye;
TH2I *ggm;
TCanvas *ca[1000];
int ic=-1;//canvas id
int ncy=1;//number of windows in y axia
int icy=1;
int ih=0;

void gg()
{
  if(f==NULL) {
    f=new TFile(fname);
    cout<<"load ROOT file: "<<fname<<endl;
  }
  xe=(TH1D*)f->Get("TpjPeak");
  xe->SetTitle(xe->GetName());
  ggm=(TH2I*)f->Get("ggmat");
  ggm->SetTitle(ggm->GetName());
  setxrange();
  setnpeaks();
  setpeakwidth();
  tpjm();
}

void tpjm()
{
  newcanvas(2);
  setxrange(0,1536);
  tpj(1);
  setxrange(1536,3072);
  tpj(2);
  setxrange();
}
void tpj(int icy=1)
{
  if(icy>ncy) icy=ncy;
  ca[ic]->cd(icy);
  TString sname=Form("%s_%i",xe->GetName(),ih++);
  TH1D *h=(TH1D*)xe->Clone(sname);
  h->SetTitle(xe->GetTitle());
  peaks(h);
}

//gated on x axis
void g(double ge, int icy=1)
{
  if(icy>ncy) icy=ncy;
  ca[ic]->cd(icy);
  TString sha=Form("gated on ge=%i",int(ge));
  TH1D *ha=(TH1D*)ggm->ProjectionX(sha,ge+dge1,ge+dge2);
  TString sname=Form("%s_%i",ha->GetName(),ih++);
  ha->SetName(sname);
  ha->SetTitle(sha);
  peaks(ha);
  
}

void gm(double ge1,double ge2=0,double ge3=0,double ge4=0, double ge5=0,double ge6=0)
{
  int npad=0;
  double ge[6]={ge1,ge2,ge3,ge4,ge5,ge6};
  for(int i=0;i<6;i++) 
    if(ge[i]>1) npad++;
  newcanvas(npad);
  for(int i=0;i<npad;i++)
    g(ge[i],i+1);
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
TH1D *hb;
void peaks(TH1 *hh, Double_t thres=0.05)
{
  TString sname=Form("%s_%i",hh->GetName(),ih++);
  double x0=hh->GetBinLowEdge(hh->GetNbinsX());
  double x1=hh->GetBinLowEdge(0);
  if(xmin<x1) xmin=x1;
  if(xmax>x0) xmax=x0;
  hh->SetAxisRange(xmin,xmax,"X");
  // hh->Print("all");
  double ymin=hh->GetMinimum();
  double ymax=hh->GetMaximum()*1.3;
  if(ymin<0) ymin=0.01;
  cout<<"a "<<hh->GetMaximum()<<endl;
  TH1D *h=(TH1D*)hh->Clone(sname); 
  h->SetAxisRange(xmin,xmax,"X");
  h->Sumw2(0);
  h->SetLineColor(kBlue);
  h->SetFillColor(kCyan);
  TSpectrum *s=new TSpectrum(500);
  h->SetAxisRange(ymin,ymax,"Y");
  h->SetStats(0);
  Int_t nfound=100;
  Int_t nloop=0;
   while(nloop<50){
    nfound=s->Search(h,2,"",thres);
    if(nfound>npeaks) thres += 0.005;
    else thres -= 0.005;
    if(thres<0 || abs(nfound-npeaks)<3 ) break;
    nloop++;
    }
  // cout<<nfound<<" peaks have been found."<<endl;
  TPolyMarker *pm=(TPolyMarker *)
                      h->GetListOfFunctions()->FindObject("TPolyMarker");
  cout<<"pm:"<<pm<<endl;
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
}

