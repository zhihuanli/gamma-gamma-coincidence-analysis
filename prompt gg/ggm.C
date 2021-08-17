
// gamma-gamma coincidence analysis code - ggm.C
// by Zhihuan Li & Hongyi Wu , Peking University.
//  Based on the code https://github.com/nijinger/rw05/blob/master/src/gf3m_subs.c
#define DATAFORMAT_M4B 0
#define DATAFORMAT_ROOT 1
#define DATAFORMAT DATAFORMAT_ROOT //定义数据格式，选择 DATAFORMAT_M4B 或者 DATAFORMAT_ROOT


#if DATAFORMAT == DATAFORMAT_M4B//如果采用 m4b 格式数据，则修改以下定义
#define BINNUMBER 4096
#define ENERGYMAX 4096  //keV
#define FILENAME_M4B  "co59.m4b"
#elif DATAFORMAT == DATAFORMAT_ROOT//如果采用 root 格式数据，则修改以下定义
#define FILENAME_ROOT "hG2t.root"
#define ROOTTH2TYPE  TH2F  //可选择 TH2I TH2F TH2D
#define ROOTTH2NAME  "hG2t"
#endif

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
// 以下程序请勿修改
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


#include "TCanvas.h"
#include "TPad.h"
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TBranch.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TF1.h"
#include "TGraph.h"
#include "TLegend.h"
#include "TMath.h"
#include "TRandom.h"
#include "TBenchmark.h"
#include "TString.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TSystem.h"

#include <fstream>
#include <iostream>
#include <set>


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
TH2 *h2=NULL;
TH1D *h1,*htmp;
TCanvas* c1;

TCanvas *ca[1000];
int ic=-1;//canvas id
int icy=1;
int ncy=1;//number of windows in y axis

int ih=0;
int ihadd=0;
int ihsub=0;
int ihand=0;

int gpeak;

std::set<int> peakbin;
std::set<int> bgbin;

double upx0=-1;
int binx1,binx2;
TBox *b;

int npeaks=30;
double hxmin;
double hxmax;

void tpj();
void gate(double,double);//set peak and background bins
void gshow(); //draw gated spectrum
void gateload(TString sname);//load gate file and fill to a histogram.
void gatewrite();//write current gate condtion to a file
void peaks(TH1* h,bool bg=false);//peaks
void newcanvas(int ncy=1);
//show multi-histograms
void gmshow(TH1* h1, TH1 *h2=NULL, TH1 *h3=NULL, TH1 *h4=NULL,TH1 *h5=NULL, TH1 *h6=NULL);
void gandshow(TH1* h1, TH1 *h2=NULL, TH1 *h3=NULL, TH1 *h4=NULL,TH1 *h5=NULL, TH1 *h6=NULL);
void gaddshow(TH1* h1, TH1 *h2=NULL, TH1 *h3=NULL, TH1 *h4=NULL,TH1 *h5=NULL, TH1 *h6=NULL);
void gsubshow(TH1* h1, TH1 *h2=NULL, TH1 *h3=NULL, TH1 *h4=NULL,TH1 *h5=NULL, TH1 *h6=NULL);
void setxrange(double hxmin1=0, double hxmax1=hxmax) {hxmin=hxmin1; hxmax=hxmax1;};
void setnpeaks(int npeaks1=30) {npeaks=npeaks1;};


void ggnew()
{
#if ROOT_VERSION_CODE < ROOT_VERSION(6,0,0)
  cout<<"ROOT 版本太低，请使用 ROOT6."<<endl;
  gApplication->Terminate(0);
#endif

  gStyle->SetCanvasPreferGL(kTRUE);
  
  c1 = new TCanvas("c1","c1");
  c1->ToggleEventStatus();//底端信息栏

  //禁止手动关闭画板
  ((TRootCanvas *)c1->GetCanvasImp())->DontCallClose();
  // ((TRootCanvas *)c1->GetCanvasImp())->SetMWMHints(kMWMDecorAll, kMWMFuncResize | kMWMFuncMaximize | kMWMFuncMinimize | kMWMFuncMove, kMWMInputModeless);// LINUX
  
#ifdef R__WIN32
  cout<<"win32"<<endl;
#endif
  
#ifdef R__UNIX
  cout<<"LINUX"<<endl;
#endif

#ifdef R__HAS_COCOA
  cout<<"MAC COCOA"<<endl;
#endif
  
#if DATAFORMAT == DATAFORMAT_M4B
  h2 = new TH2D("h2","",BINNUMBER,0,ENERGYMAX,BINNUMBER,0,ENERGYMAX);
  uint data;
  std::ifstream readdata(FILENAME_M4B,std::ios::binary);
  for (int i = 1; i <= BINNUMBER; ++i)
    for (int j = 1; j <= BINNUMBER; ++j)
      {
	readdata.read((char*)&data,sizeof(uint));
	if(data > 0)
	  {
	    h2->SetBinContent(i,j,data);
	  }
      }
  readdata.close();
#elif DATAFORMAT == DATAFORMAT_ROOT
  TFile *fin=new TFile(FILENAME_ROOT);
  h2=(ROOTTH2TYPE*)fin->Get(ROOTTH2NAME);
  h2->ClearUnderflowAndOverflow();
#endif

  hxmax = h2->GetXaxis()->GetXmax();
  hxmin = h2->GetXaxis()->GetXmin();
  h1 = (TH1D*)h2->ProjectionX("h1");
  tpj();
}

void tpj()
{
  gStyle->SetTitleAlign(33);
  gStyle->SetTitleX(.65);
  if(!c1) { c1 = new TCanvas("c1","c1"); }
  c1->DeleteExec("gateflag");
  c1->DeleteExec("bgflag");
  c1->cd();
  //h1->Draw();
  peaks(h1,0);
  h1->SetTitle("Total projection spectrum");
  c1->Draw();

  peakbin.clear();
  bgbin.clear();
}

void gate()
{
  gpeak=-1;
  peakbin.clear();
  std::cout<<"Gate: left-click on the left/right bins"<<std::endl;
  if(!c1) TCanvas *c1 = new TCanvas("c1","c1");
  c1->GetListOfPrimitives()->Remove(b);
  c1->Modified();
  c1->Draw();
  c1->cd();
  peaks(h1);
  c1->DeleteExec("bgflag");
  c1->AddExec("gateflag","gateflag()");
}

void gate(int gpeak1,double wgpeak=70)
{
  gpeak=gpeak1;
  peakbin.clear();
  std::cout<<"Gate: left-click on the left/right bins"<<std::endl;
  if(!c1) tpj();
  c1->GetListOfPrimitives()->Remove(b);
  c1->Modified();
  c1->Draw();
  c1->cd();
  int hxmin1=hxmin;
  int hxmax1=hxmax;
  setxrange(gpeak-wgpeak,gpeak+wgpeak);
  peaks(h1);
  setxrange(hxmin1,hxmax1);
  c1->DeleteExec("bgflag");
  c1->AddExec("gateflag","gateflag()");
  
}


void bg()
{
  bgbin.clear();
  std::cout<<"Background: left-click bins"<<endl;
  if(!c1)  c1 = new TCanvas("c1","c1");
  c1->Draw();
  c1->DeleteExec("gateflag");
  c1->AddExec("bgflag","bgflag()");
  
}


void gateflag()
{
  int pe = gPad->GetEvent();
  if(pe != 11) return;
  gPad->GetCanvas()->FeedbackMode(kTRUE);
  // if(gPad->GetUniqueID() == 0)
  if(upx0 <0)
    {
      gPad->SetUniqueID(gPad->GetEventX());
      upx0 = -1;
      //点击第一下
      TObject *select = gPad->GetSelected();
      if(!select) {return;}
      if (!select->InheritsFrom(TH1::Class())) {return;}
      int px = gPad->GetEventX();
      TH1 *h = (TH1*)select;
      h->Draw();
      upx0 = gPad->AbsPixeltoX(px);
      double x0 = gPad->PadtoX(upx0);
      double binx0 = h->FindBin(x0);
      
      b = new TBox(h->GetBinLowEdge(binx0),
			 gPad->GetUymin(),
			 h->GetBinWidth(binx0)+h->GetBinLowEdge(binx0),
			 h->GetBinContent(binx0));
      b->SetFillColorAlpha(kRed,0.35);
      b->Draw();

      
      gPad->Update();
    }
  else
    {
      int pxold = gPad->GetUniqueID();
      Float_t upxold = gPad->AbsPixeltoX(pxold);
      int px = gPad->GetEventX();
      int py = gPad->GetEventY();

      float uxmin = gPad->GetUxmin();
      float uxmax = gPad->GetUxmax();
      float uymin = gPad->GetUymin();
      float uymax = gPad->GetUymax();
      int pxmin = gPad->XtoAbsPixel(uxmin);
      int pxmax = gPad->XtoAbsPixel(uxmax);
      int pymin = gPad->YtoAbsPixel(uymin);
      int pymax = gPad->YtoAbsPixel(uymax);
      
      Float_t upx = gPad->AbsPixeltoX(px);
      Float_t upy = gPad->AbsPixeltoY(py);
      
      TObject *select = gPad->GetSelected();
      if(!select) {gPad->SetUniqueID(0); return;}
      if (!select->InheritsFrom(TH1::Class())) {gPad->SetUniqueID(0); return;}

      TH1 *h = (TH1*)select;

      double x1 = gPad->PadtoX(upx0);
      binx1 = h->FindBin(x1);
      x1=h->GetBinCenter(binx1);
      Float_t upx2 = gPad->AbsPixeltoX(px);
      double x2 = gPad->PadtoX(upx2);
      binx2 = h->FindBin(x2);
      x2=h->GetBinCenter(binx2);
      if(binx1 == binx2) return;
      
      if( binx1 > binx2)
	{
	  Float_t temp;
	  temp = binx1;
	  binx1 = binx2;
	  binx2 = temp;
	}
      for (int i = binx1; i <= binx2; ++i)
	{
	  peakbin.insert(i);
	  if(i == binx1) continue;
	  b = new TBox(h->GetBinLowEdge(i),
			     gPad->GetUymin(),
			     h->GetBinWidth(i)+h->GetBinLowEdge(i),
			     h->GetBinContent(i));
	  b->SetFillStyle(1001);
	  b->SetFillColorAlpha(kRed,0.50);
	  b->Draw();

	}
      
      std::cout<<x1<<" - "<<x2<<std::endl;
      gPad->Update();
      upx0 = -1;
      bg();

    }
}

void bgflag()
{
  int pe = gPad->GetEvent();
  if(pe != 11) return;
  gPad->GetCanvas()->FeedbackMode(kTRUE);    
  gPad->SetUniqueID(gPad->GetEventX());
  //点击第一下
  TObject *select = gPad->GetSelected();
  if(!select) {return;}
  if (!select->InheritsFrom(TH1::Class())) {return;}
  int px = gPad->GetEventX();
  TH1 *h = (TH1*)select;
  double upxb = gPad->AbsPixeltoX(px);
  double x0 = gPad->PadtoX(upxb);
  double binx0 = h->FindBin(x0);
  x0=h->GetBinCenter(binx0);
  bgbin.insert(binx0);
  cout<<x0<<endl;
  b = new TBox(h->GetBinLowEdge(binx0),
		     gPad->GetUymin(),
		     h->GetBinWidth(binx0)+h->GetBinLowEdge(binx0),
		     h->GetBinContent(binx0));
    b->SetFillStyle(1001);
  b->SetFillColorAlpha(kBlue,0.5);
  b->Draw();
  gPad->Update();
}



void show()
{
  std::cout<<"peakbin.size="<<peakbin.size()<<" : ";
  for(auto is=peakbin.begin();is!=peakbin.end();is++)
    cout<< *is<<" ";
  cout<<endl;
  
  std::cout<<"bgbin.size="<<bgbin.size()<<" : ";
  for(auto is=bgbin.begin();is!=bgbin.end();is++)
    cout<< *is<<" ";
  cout<<endl;
}

void gshow()
{
  gStyle->SetTitleAlign(33);
  gStyle->SetTitleX(.89);
  
  if(peakbin.size()==0) {
    cout<<"make gate with gate() first!"<<endl;
    return;
  }
  c1->DeleteExec("gateflag");
  c1->DeleteExec("bgflag");
  newcanvas();//create ca[ic];
    if(!htmp) htmp=new TH1D("hgtmp","",h2->GetYaxis()->GetNbins(),0,h2->GetYaxis()->GetXmax());
  TH1D *hg = new TH1D("hg","",h2->GetYaxis()->GetNbins(),0,h2->GetYaxis()->GetXmax());
  double weight=double(peakbin.size())/bgbin.size();

  double peak0 = h1->GetBinCenter(*peakbin.begin());
  double peak1 = h1->GetBinCenter(*peakbin.rbegin());
  for(auto is=peakbin.begin();is!=peakbin.end();is++)
    {
      htmp->Reset();
      htmp = h2->ProjectionY("",int(*is),int(*is));
      hg->Add(hg,htmp,1,1);
    }
  for(auto is=bgbin.begin();is!=bgbin.end();is++)
    {
      htmp->Reset();
      htmp = h2->ProjectionY("",int(*is),int(*is));
      hg->Add(hg,htmp,1,-weight);    
    }

  gpeak=(peak0+peak1)/2.;
  
  ca[ic]->cd();
  TString sht=Form("gated on %d keV",gpeak);
  //TString shn=Form("hg%d_%d",gpeak,ih++);
  TString shn=Form("hg%d",gpeak);  
  setxrange(0,hxmax);
  peaks(hg);
  hg->SetName(shn);
  hg->SetTitle(sht);
  ca[ic]->Draw();
}

void gatewrite()
{
  if(peakbin.size()==0) {
    cout<<"make gate with gate() first!"<<endl;
    return;
  }
  
  double peak0 = h1->GetBinCenter(*peakbin.begin());
  double peak1 = h1->GetBinCenter(*peakbin.rbegin());

  gpeak=(peak0+peak1)/2.;
  TString fname=Form("g%d.txt",gpeak);
  cout<<Form("bins for peak and backround for gpeak=%d will be write to g%d.txt",gpeak,gpeak)<<endl;
  ofstream fout(fname.Data());
  fout<<gpeak<<endl;
  fout<<peakbin.size()<<endl;
  for(auto is=peakbin.begin();is!=peakbin.end();is++)
    fout<< *is<<" ";
  fout<<endl;
  
  fout<<bgbin.size()<<endl;
  for(auto is=bgbin.begin();is!=bgbin.end();is++)
    fout<< *is<<" ";
  fout<<endl;
  fout.close();
}

void gateload(TString sname)
{
  ifstream fin(sname.Data());
  if(!fin.is_open()) {
    cout<<"file:"<<sname.Data()<<" does not exist!"<<endl;
    return;
  }

  fin>>gpeak;

  int npeak,nbg,bin;
  fin>>npeak;
  peakbin.clear();
  cout<<npeak<<endl;
  c1->GetListOfPrimitives()->Remove(b);
  c1->cd();
  c1->Draw();
  peaks(h1);
  h1->SetAxisRange(gpeak-100,gpeak+100,"X");
  c1->Modified();
  c1->Update();//聚焦到画板，避免第一个 gPad->GetUymin() 得到 0
  for(int i=0;i<npeak;i++) {
    fin>>bin;
    peakbin.insert(bin);
    b = new TBox(h1->GetBinLowEdge(bin),
		 gPad->GetUymin(),
		 h1->GetBinWidth(bin)+h1->GetBinLowEdge(bin),
		 h1->GetBinContent(bin));
    // std::cout<<gPad->GetUymin()<<" "<<h1->GetBinContent(bin)<<std::endl;
    b->SetFillStyle(1001);
    b->SetFillColorAlpha(kRed,0.50);
    b->Draw();
    gPad->Update();
    cout<<bin<<endl;
  }
  fin>>nbg;
  cout<<nbg<<endl;
  bgbin.clear();
  for(int i=0;i<nbg;i++) {
    fin>>bin;
    bgbin.insert(bin);
    b = new TBox(h1->GetBinLowEdge(bin),
		 gPad->GetUymin(),
		 h1->GetBinWidth(bin)+h1->GetBinLowEdge(bin),
		 h1->GetBinContent(bin));
    b->SetFillStyle(1001);
    b->SetFillColorAlpha(kBlue,0.50);
    b->Draw();
    gPad->Update();
    cout<<bin<<endl;
  }
  fin.close();
  
  gshow();
}

void gmshow(TH1* h1, TH1 *h2, TH1 *h3, TH1 *h4, TH1 *h5, TH1 *h6)
{
  gStyle->SetTitleAlign(33);
  gStyle->SetTitleX(.89);
  int npad=0;
  TH1 *hgm[6]={h1,h2,h3,h4,h5,h6};
  for(int i=0;i<6;i++) 
    if(hgm[i]!=NULL) npad++;
  newcanvas(npad);
  for(int i=0;i<npad;i++) {
    ca[ic]->cd(i+1);
    peaks(hgm[i]);
  }
    ca[ic]->Draw();
}

void gaddshow(TH1* h1, TH1 *h2, TH1 *h3, TH1 *h4, TH1 *h5, TH1 *h6)
{
  gStyle->SetTitleAlign(33);
  gStyle->SetTitleX(.89);
  int npad=0;
  TH1 *ha[6]={h1,h2,h3,h4,h5,h6};
  for(int i=0;i<6;i++) 
    if(ha[i]!=NULL) npad++;
  newcanvas(npad+1);
  TString sha="Add gated spectra";
  TString sname=Form("gadd%i",ihadd++);
  
  TH1 *hadd=(TH1F*)ha[0]->Clone(sname);
  hadd->Reset();
  hadd->SetTitle(sha);
  for(int i=0;i<npad;i++) {
    hadd->Add(hadd,ha[i]);
    ca[ic]->cd(i+1);
    peaks(ha[i]);
  }
  ca[ic]->cd(npad+1);
  peaks(hadd);
}

void gsubshow(TH1* h1, TH1 *h2, TH1 *h3, TH1 *h4, TH1 *h5, TH1 *h6)
{
  gStyle->SetTitleAlign(33);
  gStyle->SetTitleX(.89);
  int npad=0;
  TH1 *ha[6]={h1,h2,h3,h4,h5,h6};
  for(int i=0;i<6;i++) 
    if(ha[i]!=NULL) npad++;
  newcanvas(npad+1);
  TString sha="sub gated spectra";
  TString sname=Form("gsub%i",ihsub++);
  TH1 *hadd=(TH1F*)ha[0]->Clone(sname);
  hadd->SetTitle(sha);
  ca[ic]->cd(1);
  peaks(ha[0]);
    for(int i=1;i<npad;i++) {
   hadd->Add(ha[i],-1);
    ca[ic]->cd(i+1);
    peaks(ha[i]);
     }
  ca[ic]->cd(npad+1);
  hadd->Draw();
  peaks(hadd);
}


void gandshow(TH1* h1, TH1 *h2, TH1 *h3, TH1 *h4, TH1 *h5, TH1 *h6)
{
  gStyle->SetTitleAlign(33);
  gStyle->SetTitleX(.89);
  int npad=0;
  TH1 *ha[6]={h1,h2,h3,h4,h5,h6};
  for(int i=0;i<6;i++) 
    if(ha[i]!=NULL) npad++;
  newcanvas(npad+1);
  TString sha="and gated spectra";
  TString sname=Form("gand%i",ihand++);
  TH1 *hadd=(TH1F*)ha[0]->Clone(sname);
  hadd->Reset("M");//!!! reset maximum and minimum;
  hadd->SetTitle(sha);
  for(int i=1;i<=ha[0]->GetNbinsX();i++){
    double ma=ha[0]->GetBinContent(i);
    for(int j=1;j<npad;j++) {
      double mb=ha[j]->GetBinContent(i);
      ma=TMath::Min(ma,mb);
    }
    hadd->Fill(hadd->GetBinCenter(i),ma);
  }
  for(int i=0;i<npad;i++) {
    ca[ic]->cd(i+1);
    peaks(ha[i]);
     }
  ca[ic]->cd(npad+1);
  hadd->Draw();
  peaks(hadd);
}

void peaks(TH1 *h,bool bg)
{
  double thres=0.0001;
  h->SetLineColor(kBlue);
  h->SetFillColor(kCyan);
  h->SetAxisRange(hxmin,hxmax,"X");
  cout<<hxmin<<" "<<hxmax<<endl;
  TSpectrum *s=new TSpectrum(500);
  if(bg) {
    for(int i=1;i<=h->GetNbinsX();i++) //set bins with negative counts to zero
      if(h->GetBinContent(i)<0)  h->SetBinContent(i,0);
    TH1F *hb=(TH1F*)s->Background(h,10,"same");
    h->Add(h,hb,1,-1);
    delete hb;
  }
  h->Sumw2(0);
  h->SetStats(0);
  Int_t nfound=100;
  Int_t nloop=0;
   while(1){
    nfound=s->Search(h,2,"",thres);
    if(nfound>npeaks+5) thres += 0.0005;
    else break;
    nloop++;
    }
   cout<<"found "<<nfound<<" peaks."<<endl;
  TPolyMarker *pm=(TPolyMarker *)
                      h->GetListOfFunctions()->FindObject("TPolyMarker");
  pm->SetMarkerStyle(32);
  pm->SetMarkerColor(kGreen);
  pm->SetMarkerSize(0.4);
  Double_t *xpeaks=s->GetPositionX();
  Double_t *ypeaks=s->GetPositionY();
  int maxy=0;
  for(int k=0;k<nfound;k++)
    if(maxy<ypeaks[k])
      maxy=int(ypeaks[k]);
  for(int j=0;j<nfound;j++) {
   stringstream ss;
   ss<<xpeaks[j];
   //if(ypeaks[j]<3) continue;
   TString s1=ss.str();
   TLatex *tex=new TLatex(xpeaks[j],ypeaks[j],s1);
   tex->SetTextFont(13);
   tex->SetTextSize(13);
   tex->SetTextAlign(12);
   tex->SetTextAngle(90);
   if(maxy==int(ypeaks[j]))
     tex->SetTextAngle(0);
   tex->SetTextColor(kRed);
   tex->Draw();
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
  ca[ic]->ToggleEventStatus();
}

// 
// ggnew.cc ends here
