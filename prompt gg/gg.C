//gamma-gamma analysis for prompt gg symptotic matix
//usage:
//root -l ge.C //load root file and draw xtpj and ytpj.
//--ex.1
//newcanvas(2) -- create a cavas with (1,2)
//g(132,1) -- fill gate_132 to 1st. window of the canvas
//g(152,2) -- fill gate_152 to 2nd. window of the canvas
//--ex.2
//newcanvas() -- create a canvas with (1,1)
//g(132) -- fill gate_132 to the canvas
//--ex.3
//g(123) -- fill gate_123 to current canvas.

int xmin=0;
int xmax=3000;//range for hist
int npeaks=30;//number of peaks for TSpectrum
int dge1=-3;//gate width:ge+dge1 - ge+dge2
int dge2=3;
TString fname="46_123gg.root";//filename;

void gg();
void setxrange(int xmin1=0, int xmax1=4000) {xmin=xmin1;xmax=xmax1;};
void setnpeaks(int npeaks1=30) {npeaks=npeaks1;};
void setpeakwidth(double dgea=-3,double dgeb=3) {dge1=dgea;dge2=dgeb;};
void newcanvas(int ncy=1);
void tpj(int icy=1);
void g(double ge,int icy=1);
void peaks(TH1 *h, Double_t thres=0.05,int backsub=1);


TFile *f;
TH1D *xe,*ye;
TH2D *ggm;
TCanvas *ca[1000];
int ic=-1;//canvas id
int ncy=1;//number of windows in y axia
int icy=1;
int ih=0;

void gg()
{
  if(f==NULL) f=new TFile(fname);
  cout<<"load ROOT file: "<<fname<<endl;
  xe=(TH1D*)f->Get("TpjPeak");
  xe->SetTitle(xe->GetName());
  ggm=(TH2D*)f->Get("ggmat");
  ggm->SetTitle(ggm->GetName());
  setxrange(0,2000);
  setnpeaks(40);
  setpeakwidth(-3,3);
  newcanvas(1);
  tpj(1);
}

void tpj(int icy=1)
{
  if(icy>ncy) icy=ncy;
  ca[ic]->cd(icy);
  peaks(xe);
}

//gated on x axis
void g(double ge, int icy=1)
{
  if(icy>ncy) icy=ncy;
  ca[ic]->cd(icy);
  TString sha=Form("gated on ge=%i",int(ge));
  TH1D *ha=(TH1D*)ggm->ProjectionY(sha,ge+dge1,ge+dge2);
  TString sname=Form("%s_%i",ha->GetName(),ih++);
  TH1D *h=(TH1D*)ha->Clone(sname);
  ha->SetTitle(sha);
  peaks(ha);
  
}


void newcanvas(int ncy1=1)
{
  ic++;
  ncy=ncy1;
  int npx=800;
  int npy=800;
  if(ncy1==1)npy=400;
  ca[ic]=new TCanvas(Form("ca%i",ic),Form("canvas%i",ic),npx,npy);
  ca[ic]->Divide(1,ncy);
  if(ncy>1) cout<<Form("create a CANVAS [%s] with (1,%i) ",ca[ic]->GetName(),ncy)<<endl;

}

TH1D *hb;

void peaks(TH1 *h, Double_t thres=0.05,int backsub=0)
{

  h->SetAxisRange(xmin,xmax,"X");
  h->Sumw2(0);
  h->SetLineColor(kBlue);
  h->SetFillColor(kCyan);
  TSpectrum *s=new TSpectrum(500);
  if(backsub) {
   hb=(TH1D*)s->Background(h,20,"same");
   h->Add(h,hb,1,-1);
  }
  int ymin=h->GetMinimum();
  int ymax=h->GetMaximum()*1.2;
  if(ymin<0) ymin*=0.2;
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
  cout<<nfound<<" peaks have been found."<<endl;
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

