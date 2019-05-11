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

//gamma-gamma analysis for delayed gg asymptotic matix
//usage:
//root -l age.C //load root file and draw xtpj and ytpj.

int xmin=0;
int xmax=2000;//range for hist
int npeaks=30;//number of peaks for TSpectrum
int dge1=-3;//gate width:ge+dge1 - ge+dge2
int dge2=3;

TString fname="d46_123gg.root";

void agg();
void setxrange(int xmin1=0, int xmax1=2000) {st.xmin=xmin1;st.xmax=xmax1;};
void setnpeaks(int npeaks1=30) {st.npeaks=npeaks1;};
void setpeakwidth(double dgea=-3,double dgeb=3) {st.dge1=dgea;st.dge2=dgeb;};
void newcanvas(int ncy=1);

void atpjm();
void btpjm();
void atpj(int icy=1);
void btpj(int icy=1);

TString ga(double ge=0,int icy=1);
TString gb(double ge=0,int icy=1);
void gam(double ge1=0,double ge2=0,double ge3=0,double ge4=0,
	double ge5=0,double ge6=0);//draw multi peaks in a canvas , up to six peaks.
void gbm(double ge1=0,double ge2=0,double ge3=0,double ge4=0,
	double ge5=0,double ge6=0);//draw multi peaks in a canvas , up to six peaks.
TString peaks(TH1 *h, Double_t thres=0.05,int backsub=0);
void show(){tall->ls();};//show name of all histograms

TFile *f;
TH1D *hg2xp,*hg2yp;
TH2D *hg2xyp;
TCanvas *ca[1000];

int ic=-1;//canvas id
int ncy=1;//number of windows in y axia
int icy=1;
int ih=0;

void agg()
{
  if(f==NULL) {
    f=new TFile(fname);
    cout<<"load ROOT file: "<<fname<<endl;
  }
  hg2xp=(TH1D*)f->Get("hg2xp");
  hg2yp=(TH1D*)f->Get("hg2yp");
  hg2xyp=(TH2D*)f->Get("hg2xyp");
  setxrange();
  setnpeaks();
  setpeakwidth();
  if(tall==NULL) tall=new TList();
  atpjm();
  btpjm();
}

void btpjm()
{
  newcanvas(2);
  setxrange(0,1000);
  btpj(1);
  setxrange(1000,2000);
  btpj(2);
  setxrange();
}
 
void atpjm()
{
  newcanvas(2);
  setxrange(0,1000);
  atpj(1);
  setxrange(1000,2000);
  atpj(2);
  setxrange();
}
 
void btpj(int icy)
{
  if(icy>ncy) icy=ncy;
  ca[ic]->cd(icy);
  TString sname=Form("btpj%i",ih++);
  TH1F *h=(TH1F*)hg2xp->Clone(sname);
  peaks(h);

}
void atpj(int icy)
{
  if(icy>ncy) icy=ncy;
  ca[ic]->cd(icy);
  TString sname=Form("atpj%i",ih++);
  TH1F *h=(TH1F*)hg2yp->Clone(sname);
  peaks(h); 
}

//gated on gamma decay from mother
TString ga(double ge,int icy1)
{
  if(ge==0) {
    cout<<"wrong parameters!"<<endl;
    return "error";
  }
  if(icy1>ncy) icy=ncy;
  else icy=icy1;
  ca[ic]->cd(icy);
  TString sha=Form("gated on ae=%.1f keV",ge);
  double gea=hg2xyp->GetXaxis()->FindBin(ge+st.dge1);
  double geb=hg2xyp->GetXaxis()->FindBin(ge+st.dge2); 
  TH1F *ha=(TH1F*)hg2xyp->ProjectionX(Form("ga%d_%i",int(ge),ih++),gea,geb);
  ha->SetTitle(sha);
  tall->Add(ha);
  return peaks(ha);

}

//gated on gamma decay from daughter
TString gb(double ge,int icy1)
{
  if(ge==0) {
    cout<<"wrong parameters!"<<endl;
    return "error";
  }
  if(icy1>ncy) icy=ncy;
  else icy=icy1;
  ca[ic]->cd(icy);
  TString sha=Form("gated on be=%.1f keV",ge);
  double gea=hg2xyp->GetXaxis()->FindBin(ge+st.dge1);
  double geb=hg2xyp->GetXaxis()->FindBin(ge+st.dge2); 
  TH1F *ha=(TH1F*)hg2xyp->ProjectionY(Form("gb%d_%i",int(ge),ih++),gea,geb);
  ha->SetTitle(sha);
  tall->Add(ha);
  return peaks(ha);
  
  
}

void gbm(double ge1,double ge2,double ge3,double ge4, double ge5,double ge6)
{
    int npad=0;
  double ge[6]={ge1,ge2,ge3,ge4,ge5,ge6};
  for(int i=0;i<6;i++) 
    if(ge[i]>1) npad++;
  newcanvas(npad);
  for(int i=0;i<npad;i++)
    gb(ge[i],i+1);
  
}

void gam(double ge1,double ge2,double ge3,double ge4, double ge5,double ge6)
{
    int npad=0;
  double ge[6]={ge1,ge2,ge3,ge4,ge5,ge6};
  for(int i=0;i<6;i++) 
    if(ge[i]>1) npad++;
  newcanvas(npad);
  for(int i=0;i<npad;i++)
    ga(ge[i],i+1);

}
 
void newcanvas(int ncy1)
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

TString peaks(TH1 *h, Double_t thres,int backsub)
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

