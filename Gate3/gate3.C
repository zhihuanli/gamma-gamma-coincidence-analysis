
#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TList.h"
#include "TString.h"
#include "TSpectrum.h"
#include "TLatex.h"
#include "TColor.h"
#include "TPolyMarker.h"
#include "TApplication.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TRootCanvas.h"
#include "TLine.h"

#include <map>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#define codename         "Gamma-Gamma three-fold matrix data analysis Code GATE3"
#define author           "Author: Zhihuan Li, Peking University"
#define lastmodified     "Last modified on Dec. 18, 2021"
#define webpage          "Find the last updates from [https://github.com/zhihuanli/gamma-gamma-coincidence-analysis/tree/master/Gate2]"

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */                                                                
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */

struct parameters
{
  TString g3file;
  TString prefixhist;
  TString g3bkgfile;
  TString xyhist;
  TString xhist;
  TString xbkghist;
  int     ncanvas;
  int     xmin;    //range for hist
  int     xmax;    //range for hist
  int     npeaks;  //number of peaks for TSpectrum
  float   dge;    //gate width:ge-dge - ge+dge

};
parameters st;

map<TString,int> mg;
map<TString,TString> mgtitle;
map<TString,int>::iterator img;
map<TString,TString>::iterator imgtitle;

TList *tlhist,*tlcanv;

int xmin=0;
int xmax;

TFile *f;
TH1I *hg3x;//1d projection
TH1I *hg3xb;//bkg of 1d projection
TH2I *hg3xy;//2d projection

TCanvas *ca[1000];
int ic=-1;//canvas id
int icy=1;
int ncy=1;//number of windows in y axia
int ih=0;

void setncanvas(int ncanvas=5) { st.ncanvas=ncanvas; }
void setxrange(int xmin1=xmin, int xmax1=xmax){ st.xmin=xmin1; st.xmax=xmax1;};
void setnpeaks(int npeaks1=30) {st.npeaks=npeaks1;};
void setgatewidth(float dgea=-2,float dgeb=2) {st.dge=dgea;};
void showSettings();

//ge1 & ge2 gated-spectrum
void g2m(float ge0a,float ge0b,float ge1a=-1,float ge1b=-1,
	   float ge2a=-1,float ge2c=-1, float ge3a=-1, float ge3b=-1);
void g2m(TH1* h1, TH1 *h2=NULL, TH1 *h3=NULL, TH1 *h4=NULL);

void g2mxr(int xmin,int xmax,float ge0a,float ge0b,
	   float ge1a=-1,float ge1b=-1, float ge2a=-1,float ge2c=-1, float ge3a=-1, float ge3b=-1);
void g2mxr(int xmin,int xmax,TH1* h1, TH1 *h2=NULL, TH1 *h3=NULL, TH1 *h4=NULL);
//total projection sepctra
void tpjm(int npad=3);
void tpjmxr(int xmin,int xmax,int npad=3);

//help
void help();


//show all existing gated histograms
void histlist(){
  cout<<GREEN<<endl;
  tlhist->ls();
  cout<<RESET<<endl;
 };//show name of all histograms

//show all existing TCanvas
void canvlist(){
  cout<<GREEN<<endl;
  tlcanv->ls();
  cout<<RESET<<endl;
 };

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//private functions
bool ReadSettings(TString fname);
//two-fold gated
TString g2(double gea,double geb,int icy1=1);
void newcanvas(int ncy,TString sctitle);
TString GetHistName(TString sprefix);
void tpj(int icy=1);
//mark peak positions
void peaks(TH1 *h, Double_t thres=0.05);
Int_t getv(TH2I *h,int i, int j) { return h->GetBinContent(i,j);}
Int_t getv(TH1I *h,int i) { return h->GetBinContent(i);}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void gate3()
{
#if ROOT_VERSION_CODE < ROOT_VERSION(6,0,0)
  cout<<"This code is valid only for  ROOT6 !"<<endl;
  gApplication->Terminate(0);
#endif
#ifdef R__WIN32
  cout<<"win32"<<endl;
#endif
  
#ifdef R__UNIX
  cout<<"LINUX"<<endl;
#endif

#ifdef R__HAS_COCOA
  cout<<"MAC COCOA"<<endl;
#endif  
  TColor::SetColorThreshold(0.1);
  cout<<endl;
  cout<< CYAN<<codename <<endl;
  cout<< YELLOW<<author <<endl;
  cout<< YELLOW<<lastmodified <<endl;
  cout<< YELLOW<<webpage <<endl;
  cout<< RESET<<endl;
  bool bvalid=ReadSettings("gate3set.txt");
  if(!bvalid)  gApplication->Terminate(0);
  if(gSystem->AccessPathName(st.g3file.Data())){
    cout <<RED<<"ROOT file: "<<st.g3file.Data()<<" does not exist" << endl;
    gApplication->Terminate(0);
  }
  xmax=0;
  
  f=new TFile(st.g3file.Data());
  TFile *fproj=new TFile(st.g3bkgfile.Data());
  if(!(hg3xy=(TH2I*)fproj->Get(st.xyhist.Data())) ||
     !(hg3x=(TH1I*)fproj->Get(st.xhist.Data())) ||
     !(hg3xb=(TH1I*)fproj->Get(st.xbkghist.Data()))) {
     cout<<RED<<"ERROR!  No histograms in "<<st.g3bkgfile.Data()<<" is detected !"<<RESET<<endl;
     gApplication->Terminate(0);
  }
    xmax=hg3x->GetBinCenter(hg3x->GetXaxis()->GetNbins());
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
  setncanvas(st.ncanvas);
  setxrange(st.xmin,st.xmax);
  setnpeaks(st.npeaks);
  setgatewidth(st.dge);
  showSettings();
  cout<<RED<<"help() for short instructions "<<RESET<<endl<<endl;
  
  if(tlhist==NULL) tlhist=new TList();
  if(tlcanv==NULL) tlcanv=new TList();  
  tpjm();
  // if(aae!=NULL) gaem();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void tpjm(int npad)
{
    if(npad>6) npad=6;
  newcanvas(npad,"Total Projection Spectrum");
  int xmina=st.xmin;
  int xmaxa=st.xmax;
  int dx=(xmaxa-xmina)/npad;
  for(int i=0;i<npad;i++) {
    setxrange(xmina+i*dx,xmina+(i+1)*dx);
    tpj(i+1);
  }
  setxrange(xmina,xmaxa);
  
}
void tpjmxr(int xmin,int xmax,int npad)
{
  int x1=st.xmin;
  int x2=st.xmax;
  setxrange(xmin,xmax);
  tpjm(npad);
  st.xmin=x1;
  st.xmax=x2;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void g2mxr(int xmin,int xmax,float ge0a,float ge0b,float ge1a,float ge1b, float ge2a,float ge2b, float ge3a, float ge3b)
{
  int x1=st.xmin;
  int x2=st.xmax;
  setxrange(xmin,xmax);
  g2m(ge0a,ge0b,ge1a,ge1b,ge2a,ge2b,ge3a,ge3b);
  st.xmin=x1;
  st.xmax=x2;

}

void g2m(float ge0a,float ge0b,float ge1a,float ge1b, float ge2a,float ge2b, float ge3a, float ge3b)
{
  int npad=0;
  double gea[4]={ge0a,ge1a,ge2a,ge3a};
  double geb[4]={ge0b,ge1b,ge2b,ge3b};
  for(int i=0;i<4;i++) 
    if(gea[i]>1) npad++;
  TString sctitle=Form("g2m(%.1f,%.1f",gea[0],geb[0]);
  for(int i=1;i<npad;i++) 
    sctitle+=Form(",%.1f,%.1f",gea[i],geb[0]);
  sctitle+=")";

  newcanvas(npad,sctitle);
  for(int i=0;i<npad;i++)
    g2(gea[i],geb[i],i+1);
}

//two fold - gated on x axis
TString g2(double gea, double geb,int icy1)
{
  if(gea==0 || geb==0) {
    cout<<"wrong parameters!"<<endl;
    return "error";
  }
  if(icy1>ncy) icy=ncy;
  else icy=icy1;
  ca[ic]->cd(icy);

  int geaL=hg3xy->GetXaxis()->FindBin(gea-st.dge);
  int geaR=hg3xy->GetXaxis()->FindBin(gea+st.dge);
  if(geaL<xmin) gea=xmin;
  if(geaR>xmax) geb=xmax;
  int gebL=hg3xy->GetXaxis()->FindBin(geb-st.dge);
  int gebR=hg3xy->GetXaxis()->FindBin(geb+st.dge);
  if(gebL<xmin) gea=xmin;
  if(gebR>xmax) geb=xmax;
  TString stitle=Form("Gated on %.1f keV & %.1f keV",gea,geb);  
  TString sname=GetHistName(Form("gg%d_%d",int(gea),int(geb)));
  TString h2name=Form("%s%04d",st.prefixhist.Data(),geaL-1);
  TH2I *hhxy=(TH2I*)f->Get(h2name.Data());
  TH1I *ha=(TH1I*)hhxy->ProjectionX(sname.Data(),gebL,gebR);
  for(int i=geaL+1;i<=geaR;i++){
    h2name=Form("%s%04d",st.prefixhist.Data(),i-1);
    hhxy=(TH2I*)f->Get(h2name.Data());
    TH1I* hb=(TH1I*)hhxy->ProjectionX("hb",gebL,gebR);    
    ha->Add(ha,hb,1,1);
    delete hb;
  }
  delete hhxy;
  ha->SetTitle(stitle);
  //Background subtraction
  TH1I *hgb=(TH1I*)hg3x->Clone("hgb");
  hgb->Reset();
  Double_t T,Mij,Mjk,Mik,Pi,Pj,Pk,bi,bj,bk,Bijk;
  T=hg3x->Integral();
  int N=hg3x->GetNbinsX();
  for(int i=1; i<=N; i++) {
    for(int j=geaL; j<=geaR; j++) {
      for(int k=gebL; k<=gebR; k++) {
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
  delete hgb;
  if(!tlhist->FindObject(ha)) tlhist->Add(ha);
  peaks(ha);
  return sname;
  
}

void g2mxr(int xmin,int xmax,TH1* h1, TH1 *h2, TH1 *h3, TH1 *h4)
{
  int x1=st.xmin;
  int x2=st.xmax;
  setxrange(xmin,xmax);
  g2m(h1,h2,h3,h4);
  st.xmin=x1;
  st.xmax=x2;

}

void g2m(TH1* h1, TH1 *h2, TH1 *h3, TH1 *h4)
{
  int npad=0;
  TH1 *hg[4]={h1,h2,h3,h4};
  map<int,int> mg[4];
  for(int i=0;i<4;i++) 
    if(hg[i]!=NULL) npad++;
  TString sctitle=Form("g2m(%s",hg[0]->GetName());
  for(int i=1;i<4;i++) 
    if(hg[i]!=NULL) sctitle+=Form(",%s",hg[i]->GetName());
  sctitle+=")";
  newcanvas(npad,sctitle);

  for(int i=0;i<npad;i++) {
    ca[ic]->cd(i+1);
    peaks(hg[i]);
  }
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......



void saveHist(TH1* hist, TString sfile)
{
  TFile *fout=new TFile(sfile.Data(),"UPDATE");
  hist->Write();
  fout->Close();
  cout<<"Write "<<hist->GetName()<<" to "<<sfile.Data()<<endl;
}

void saveHistAll(TString sfile)
{
  TFile *fout=new TFile(sfile.Data(),"UPDATE");
  tlhist->Write();
  fout->Close();
  cout<<"Write all existing histograms to "<<sfile.Data()<<endl;  
}

void saveHistData(TH1* hist, TString sfile)
{
  ofstream fout(sfile.Data());
  for(int i=1;i<=hist->GetNbinsX();i++) {
    fout<<Form("%.1f   %.1f",hist->GetBinCenter(i),hist->GetBinContent(i))<<endl;
  }
  fout.close();
  cout<<"Write "<<hist->GetName()<<" to "<<sfile.Data()<<endl;
}

void saveCanv(TCanvas* c1, TString sfile)
{
  TFile *fout=new TFile(sfile.Data(),"UPDATE");
  c1->Write();
  fout->Close();
  cout<<"Write "<<c1->GetName()<<" to "<<sfile.Data()<<endl;
}

void saveCanvAll(TString sfile)
{
  TFile *fout=new TFile(sfile.Data(),"UPDATE");
  TIter next(tlcanv);
  while (TCanvas* cc=(TCanvas*)next()) cc->Write();
  //tlcanv->Write();//some error messages when browse through TCanvas in the root file in MacOS.
  fout->Close();
  cout<<"Write all existing histograms to "<<sfile.Data()<<endl;  
}


void showSettings()
{

  cout<<GREEN<<"g3hist ROOT file: " <<st.g3file.Data()<<endl;
  cout<<GREEN<<"g3bkghist ROOT file: "<<st.g3bkgfile.Data()<<endl;
  cout<<GREEN<<Form("Range of X/Y: %d - %d ",xmin,xmax)<<RESET<<endl<<endl;
  cout<<RED<<"Current settings:"<<endl;
  cout<<CYAN<<"ncanvas = "<<st.ncanvas<<endl;
  cout<<CYAN<<"xmin = "<<st.xmin<<", xmax= "<<st.xmax<<endl;
  cout<<CYAN<<"npeaks = "<<st.npeaks<<endl;
  cout<<CYAN<<"dge = "<<st.dge<<endl;
  cout<<RESET<<endl;
}


void quit()
{
  gApplication->Terminate(0);
}

void help()
{
  cout<<YELLOW<<"....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......"<<endl;
  cout<< RED<<"Setup, "<<RED<<"changes will take effect for the next drawing."<<endl;
  cout<< CYAN<< "   setxrange(int xmin1, int xmax1)        - Change the viewing range of x-axis for all histograms."<<endl;
  cout<< "   setnpeaks(int npeaks1=30)              - Change the number of peaks marked in a histogram."<<endl;
  cout<< "   setncanvas(int ncanvas=5)              - Change the maximum number of canvas avaliable."<<endl;
  cout<< "   setgatewidth(int dge=2)                - Change range of gate to peak-dge to peak+dge."<<endl;
  cout<< "   showSettings()                         - Show current parameter settings."<<endl;
  
  cout<<YELLOW<<"....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......"<<endl;
  cout<<CYAN<<  "   g2(float ge1, float ge2)"<<endl;
  cout<<CYAN<< "   gm(TH1* h1, [TH1* h2],[TH1* h3],[TH1* h4])"<<endl;
  cout<<RED<< "Show total projection sepctrum."<<endl;
  cout<<CYAN<< "   tpjm(int npad)"<<endl;

  cout<<RED<<"Show gated spectra with specified the x-axis range."<<endl;
  cout<<CYAN<<  "   g2xr(int xmin,int xmax,double ge1, double ge2)"<<endl;
  cout<<CYAN<<  "   gmxr(int xmin,int xmax,TH1* h1, [TH1* h2], ...)"<<endl;
  cout<<CYAN<< "   tpjmxr(int xmin,int xmax,int npad)"<<endl;

  cout<<YELLOW<<"....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......"<<endl;
  cout<<RED<<"Save a hist to a ROOT file with append mode"<<endl;
  cout<<CYAN<<"    saveHist(TH1* hist, TString sfile)"<<endl;
  cout<<RED<<"Save a canvas to a ROOT file with append mode"<<endl;
  cout<<CYAN<<"    saveCanv(TCanvas* c1, TString sfile)"<<endl;
  cout<<RED<<"Save a hist data to a text file"<<endl;
  cout<<CYAN<<"    saveHistData(TH1* c1, TString sfile)"<<endl;
  cout<<RED<<"Save All histograms to a ROOT file"<<endl;
  cout<<CYAN<<"    saveHistAll(TString sfile)"<<endl;
    cout<<RED<<"Save All TCanvas to a ROOT file"<<endl;
  cout<<CYAN<<"    saveCanvAll(TString sfile)"<<endl;
  
  cout<<RED<<"Show all existing gated histograms"<<endl;
  cout<<CYAN<<"   histlist()"<<endl;

  cout<<RED<<"Show all existing TCanvas"<<endl;
  cout<<CYAN<<"   canvlist()"<<endl;

  cout<<RED<<"Safe way to quit the program in mac."<<endl;
  cout<<CYAN<<"   quit()"<<endl;
    
  cout<<RESET<<endl;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//private functions
bool ReadSettings(TString fname)
{
  // ####Gate3 initial settings ####
  // g3file      g3hist.root 
  // prefixHist  h3xy
  // g3bkgfile   g3bkg.root
  // xyhist      h3xy
  // xhist       h3x
  // xbkghist    h3xb        
  // ncanvas     5              
  // xmin        0
  // xmax        2000         
  // npeaks      30                
  // dge         2                 

  ifstream fset(fname.Data());
  if(!fset.good()) {
    cout<<fname.Data()<<" does not exist!"<<endl;
    return false;
  }
  string stmp;
  char ctmp[1000];
  getline(fset,stmp);
  fset>>stmp>>st.g3file;
  fset>>stmp>>st.prefixhist;
  fset>>stmp>>st.g3bkgfile;
  fset>>stmp>>st.xyhist;
  fset>>stmp>>st.xhist;
  fset>>stmp>>st.xbkghist;
  fset>>stmp>>st.ncanvas;
  fset>>stmp>>st.xmin;
  fset>>stmp>>st.xmax;
  fset>>stmp>>st.npeaks;
  fset>>stmp>>st.dge;
  return true;
}
void newcanvas(int ncy1,TString sctitle)
{
  ic++;
  ic=ic%5;//st.ncanvas;
  ncy=ncy1;
  double w=800;
  double h=800;
  if(ncy1==1)h=400;
  sctitle=Form("%s_canv%d",sctitle.Data(),ic);
  if(TCanvas *cc=(TCanvas*)gROOT->FindObject(Form("canv%d",ic))) {
    tlcanv->Remove(cc);
    delete cc;
  }
  ca[ic]=new TCanvas(Form("canv%i",ic),sctitle.Data(),w,h);

  ca[ic]->Divide(1,ncy);
  for(int i=1;i<=ncy;i++) {
    ca[ic]->GetPad(i)->SetBottomMargin(0.05);
    ca[ic]->GetPad(i)->SetTopMargin(0.005);
    ca[ic]->GetPad(i)->SetLeftMargin(0.08);
    ca[ic]->GetPad(i)->SetRightMargin(0.05);
  }
  ca[ic]->ToggleEventStatus();
  tlcanv->Add(ca[ic]);
    ((TRootCanvas *)ca[ic]->GetCanvasImp())->DontCallClose();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//generate histogram name according to prefix and title 
TString GetHistName(TString sprefix)
{

  img=mg.find(sprefix);
  int gid;
  if(img!=mg.end()){
      img->second++; 
  }
  else {
    gid=0;
    mg.insert(make_pair(sprefix,gid));
    img=mg.find(sprefix);
  }
  return Form("%s_%d",(img->first).Data(),img->second);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void tpj(int icy)
{
  if(icy>ncy) icy=ncy;
  ca[ic]->cd(icy);
  TString stitle=Form("Total Projection Sepctrum (%d - %d) keV ",st.xmin,st.xmax);
  TString sname=GetHistName("gtpj");   
  TH1F *h=(TH1F*)hg3xy->ProjectionX(sname.Data(),xmin,xmax);
  h->SetTitle(stitle.Data());
  if(!tlhist->FindObject(h)) tlhist->Add(h);
  peaks(h);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
    


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void peaks(TH1 *h, Double_t thres)
{
  double x0=h->GetBinLowEdge(h->GetNbinsX());
  double x1=h->GetBinLowEdge(0);
  if(st.xmin<x1) st.xmin=x1;
  if(st.xmax>x0) st.xmax=x0;
  h->SetAxisRange(st.xmin,st.xmax,"X");
  double ymin=h->GetBinContent(h->GetMinimumBin());
  double ymax=h->GetBinContent(h->GetMaximumBin())*1.3;
  if(ymin<0 && ymin<-0.1*ymax) ymin=-0.1*ymax;//ymin=0.01;
  cout<< CYAN << ic<<" "<<icy<<" "<<h->GetName()<<"  ["<<h->GetTitle()<<"]"<<RESET<<endl;
  h->GetYaxis()->SetNoExponent();
  h->SetLineColor(kBlue);
  h->SetFillColorAlpha(kCyan,0.15);
  TSpectrum *s=new TSpectrum(1000);
  h->Sumw2(0);
  h->SetAxisRange(ymin,ymax,"Y");
  h->SetStats(0);
  Int_t nfound=0;
  Int_t nloop=0;
  while(nloop<1000){
    nfound=s->Search(h,1.5,"",thres);
    if(nfound>st.npeaks) thres += 0.0001;
    else thres -= 0.0001;
    if(thres<0 || abs(nfound-st.npeaks)<3 ) break;
    nloop++;
  }
  TPolyMarker *pm=(TPolyMarker *)
                      h->GetListOfFunctions()->FindObject("TPolyMarker");
  pm->SetMarkerStyle(2);//32
  pm->SetMarkerColor(kRed);
  pm->SetMarkerSize(0.5);//0.4
  Double_t *xpeaks=s->GetPositionX();
  Double_t *ypeaks=s->GetPositionY();
  //to improve the accuracy of peak positions.
  for(int i=0;i<1;i++) {
    for(int j=0;j<nfound;j++) {
      int binx0=h->FindBin(xpeaks[j]);
      if(binx0>1 && binx0<xmax) {
	float ymax=ypeaks[j];
	float ya=h->GetBinContent(binx0-1);
	float yb=h->GetBinContent(binx0+1);
	if(ymax<ya){
	  xpeaks[j]=h->GetBinCenter(binx0-1);
	  ypeaks[j]=ya;
	  pm->SetPoint(j,xpeaks[j],ypeaks[j]);	
	}
	if(ymax<yb){
	  xpeaks[j]=h->GetBinCenter(binx0+1);
	  ypeaks[j]=yb;
	  pm->SetPoint(j,xpeaks[j],ypeaks[j]);    
	}
      }
    }
  }
  map<int,int> mg;
  for(int j=0;j<nfound;j++) {
    stringstream ss;
    ss<<xpeaks[j];
    if(ypeaks[j]<3) continue;
    TString s1=ss.str();
    TLatex *tex=new TLatex(xpeaks[j],ypeaks[j]+ymax*0.02,s1);
    tex->SetTextFont(133);//13
    tex->SetTextSize(16);
    tex->SetTextAlign(12);
    tex->SetTextAngle(90);
    tex->SetTextColor(kRed);
    tex->Draw();
    h->GetListOfFunctions()->Add(tex);
    mg.insert(make_pair(xpeaks[j],ypeaks[j]));
  }
  pm->Draw();
  TLine *l1=new TLine(st.xmin,0,st.xmax,0);
  l1->SetLineColorAlpha(kRed, 0.7);
  l1->Draw();
  ca[ic]->Draw();
}
 
