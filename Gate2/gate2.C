
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
#include "TROOT.h"
#include <map>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#define codename         "Gamma-Gamma two-fold matrix data analysis Code GATE2"
#define author           "By Zhihuan Li,  Peking University"
#define lastmodified     "Last modified on Dec. 4, 2021"
#define webpage          "Find the last updates from https://github.com/zhihuanli/gamma-gamma-coincidence-analysis/tree/master/Gate2"

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
  int ncanvas;
  int xmin;    //range for hist
  int xmax;    //range for hist
  int npeaks;  //number of peaks for TSpectrum
  float dge1;    //gate width:ge+dge1 - ge+dge2
  float dge2;
  bool peak_background;//true: all negative counts will be set to 0 
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
TH1 *hg2x;
TH2 *hg2xy;

TCanvas *ca[1000];
int ic=-1;//canvas id
int icy=1;
int ncy=1;//number of windows in y axia
int ih=0;

void setncanvas(int ncanvas=5) { st.ncanvas=ncanvas; }
void setxrange(int xmin1=xmin, int xmax1=xmax){ st.xmin=xmin1; st.xmax=xmax1;};
void setnpeaks(int npeaks1=30) {st.npeaks=npeaks1;};
void setgatewidth(float dgea=-2,float dgeb=2) {st.dge1=dgea; st.dge2=dgeb;};
void setpeakbackground(bool pb=false) {st.peak_background=pb;};
void showSettings();
// Show different parts of a gated-spectrum in the same window
void gs(float ge1,int npad=4);
void gsxr(int xmin,int xmax, float ge1,int npad=4);
//gated-spectra up to six peaks - draw existing histograms
void gm(float ge1,float ge2=-1,float ge3=-1,float ge4=-1);
void gm(TH1* h1, TH1 *h2=NULL, TH1 *h3=NULL, TH1 *h4=NULL);
void gmxr(int xmin,int xmax,float ge1,float ge2=-1,float ge3=-1,float ge4=-1);
void gmxr(int xmin,int xmax,TH1* h1, TH1 *h2=NULL, TH1 *h3=NULL, TH1 *h4=NULL);
//total projection sepctra
void tpjm(int npad=3);
void tpjmxr(int xmin,int xmax,int npad=3);
//two-fold gated with a specified gamma peak range
void gw(float ge1,float ge2);
void gwxr(int xmin,int xmax,float ge1,float ge2);
//and
void gand(float ge1,float ge2,float ge3=-1,float ge4=-1);
void gandxr(int xmin,int xmax,float ge1, float ge2, float ge3=-1, float ge4=-1);
//add 
void gadd(float ge1,float ge2,float ge3=0,float ge4=0,float ge5=0,float ge6=0);
void gaddxr(int xmin,int xmax,float ge1,float ge2,float ge3=-1,float ge4=-1,float ge5=-1,float ge6=-1);
//sub
void gsub(float ge1,float ge2,float ge3=-1,float ge4=-1);
void gsubxr(int xmin,int xmax,float ge1,float ge2,float ge3=-1,float ge4=-1);

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
//two-fold gated
TString g(double ge=0,int icy1=1);
void newcanvas(int ncy,TString sctitle);
TString GetHistName(TString sprefix);
void tpj(int icy=1);
//mark peak positions
void peaks(TH1 *h, Double_t thres=0.05);


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void gate2(TString fname="hgglmatrix.root",TString sg2="hg2xyp")
{
  TColor::SetColorThreshold(0.1);
  cout<<endl;
  cout<< CYAN<<codename <<endl;
  cout<< YELLOW<<author <<endl;
  cout<< YELLOW<<lastmodified <<endl;
  cout<< YELLOW<<webpage <<endl;
  cout<< RESET<<endl;
  if(f==NULL) {
    f=new TFile(fname);
    cout<<GREEN<<"ROOT file: "<<fname<<RESET<<endl;
  }

#if ROOT_VERSION_CODE < ROOT_VERSION(6,0,0)
  cout<<"This code is valid only for  ROOT6 !"<<endl;
  gApplication->Terminate(0);
#endif

  //two-fold
  // hg2xy=(TH2*)f->Get(sg2.Data());

  
  if(!(hg2xy=(TH2*)f->Get(sg2.Data()))) {
    cout<<RED<<"ERROR!  No 2D-histogram in "<<fname<<" is detected !"<<RESET<<endl;
     gApplication->Terminate(0);
  }
  else cout<<GREEN<<"2D-histogram: "<<sg2.Data()<<endl;
  hg2x=(TH1*) hg2xy->ProjectionX("hg2x");
  
  xmax=hg2x->GetBinCenter(hg2x->GetXaxis()->GetNbins());
  cout<<GREEN<<Form("Range of X/Y: %d - %d ",xmin,xmax)<<RESET<<endl<<endl;

  
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
  setncanvas(5);
  setnpeaks(30);
  setxrange(xmin,xmax);
  setgatewidth(-2,2);
  setpeakbackground(0);//

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
void gsxr(int xmin,int xmax, float ge1,int npad)
{
  int x1=st.xmin;
  int x2=st.xmax;
  setxrange(xmin,xmax);
  gs(ge1,npad);
  st.xmin=x1;
  st.xmax=x2;
}
void gs(float ge1,int npad)
{
  if(npad>6) npad=6;
  TString sctitle=Form("gs(%.1f)",ge1);
  newcanvas(npad,sctitle);
  int xmina=st.xmin;
  int xmaxa=st.xmax;
  int dx=(xmaxa-xmina)/npad;
  cout<<dx<<endl;
  for(int i=0;i<npad;i++) {
    int x1=xmina+i*dx;
    int x2=xmina+(i+1)*dx;
    setxrange(x1,x2);
    g(ge1,i+1);
  }
  setxrange(xmina,xmaxa);
}
void gwxr(int xmin,int xmax,float ge1,float ge2)
{
  int x1=st.xmin;
  int x2=st.xmax;
  setxrange(xmin,xmax);
  gw(ge1,ge2);
  st.xmin=x1;
  st.xmax=x2;
}
void gw(float ge1,float ge2)
{
  if(ge1<xmin) ge1=xmin;
  if(ge2>xmax) ge2=xmax;
  TString sctitle=Form("gw(%.1f,%.1f)",ge1,ge2);  
  newcanvas(1,sctitle);
  TString stitle=Form("Gated on %.1f-%.1f keV",ge1,ge2);
  int gea=hg2xy->GetXaxis()->FindBin(ge1);
  int geb=hg2xy->GetXaxis()->FindBin(ge2);

  TString sname=GetHistName(Form("gw%d-%d",int(ge1),int(ge2)));
  TH1F *ha=(TH1F*)hg2xy->ProjectionX(sname.Data(),gea,geb);
  ha->SetTitle(stitle);
  if(!tlhist->FindObject(ha)) tlhist->Add(ha);
  peaks(ha);
}

void gmxr(int xmin,int xmax,float ge1,float ge2,float ge3,float ge4)
{
  int x1=st.xmin;
  int x2=st.xmax;
  setxrange(xmin,xmax);
  gm(ge1,ge2,ge3,ge4);
  st.xmin=x1;
  st.xmax=x2;

}

void gm(float ge1,float ge2,float ge3,float ge4)
{
  int npad=0;
  double ge[4]={ge1,ge2,ge3,ge4};
  for(int i=0;i<6;i++) 
    if(ge[i]>1) npad++;
  TString sctitle=Form("gm(%.1f",ge[0]);
  for(int i=1;i<4;i++) 
    if(ge[i]>0) sctitle+=Form(",%.1f",ge[i]);
  sctitle+=")";
  newcanvas(npad,sctitle);
  for(int i=0;i<npad;i++)
    g(ge[i],i+1);
}

void gmxr(int xmin,int xmax,TH1* h1, TH1 *h2, TH1 *h3, TH1 *h4)
{
  int x1=st.xmin;
  int x2=st.xmax;
  setxrange(xmin,xmax);
  gm(h1,h2,h3,h4);
  st.xmin=x1;
  st.xmax=x2;

}

void gm(TH1* h1, TH1 *h2, TH1 *h3, TH1 *h4)
{
  int npad=0;
  TH1 *hg[4]={h1,h2,h3,h4};
  map<int,int> mg[4];
  for(int i=0;i<4;i++) 
    if(hg[i]!=NULL) npad++;
  TString sctitle=Form("gm(%s",hg[0]->GetName());
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
void gandxr(int xmin,int xmax,float ge1, float ge2, float ge3, float ge4)
{
  int x1=st.xmin;
  int x2=st.xmax;
  setxrange(xmin,xmax);
  gand(ge1, ge2, ge3, ge4);
  st.xmin=x1;
  st.xmax=x2;

}
void gand(float ge1, float ge2, float ge3, float ge4)
{
  int npad=0;
  TH1F *ha[4];
  double ge[4]={ge1,ge2,ge3,ge4};
  for(int i=0;i<4;i++) 
    if(ge[i]>1) npad++;
  
  TString sctitle=Form("gand(%.1f",ge[0]);
  for(int i=1;i<4;i++) 
    if(ge[i]>0) sctitle+=Form(",%.1f",ge[i]);
  sctitle+=")";
  
  newcanvas(npad+1,sctitle);

  TString stitle="And gates of";
  for(int i=0;i<npad;i++) {
    TString hname=g(ge[i],i+1);//return name of histogram.
    ha[i]=(TH1F*)gROOT->FindObject(hname);
    stitle=Form("%s %.1f",stitle.Data(),ge[i]);
  }
  stitle=Form("%s keV",stitle.Data());
  TString sname=GetHistName("gand");  
  TH1F *hand=(TH1F*)hg2xy->ProjectionX(sname.Data(),xmin,xmax);//copy hist structure
  hand->SetTitle(stitle);
  hand->Reset("M");//!!! reset maximum and minimum;
  for(int i=1;i<=ha[0]->GetNbinsX();i++){
    double ma=ha[0]->GetBinContent(i);
    for(int j=1;j<npad;j++) {
      double mb=ha[j]->GetBinContent(i);
      ma=TMath::Min(ma,mb);
    }
    hand->SetBinContent(i,ma);
  }
  if(!tlhist->FindObject(hand)) tlhist->Add(hand);
 
  ca[ic]->cd(npad+1);
  icy=npad+1;
  peaks(hand);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void gaddxr(int xmin,int xmax,float ge1, float ge2, float ge3, float ge4, float ge5, float ge6)
{
  int x1=st.xmin;
  int x2=st.xmax;
  setxrange(xmin,xmax);
  gadd(ge1, ge2, ge3, ge4,ge5,ge6);
  st.xmin=x1;
  st.xmax=x2;

}
void gadd(float ge1, float ge2, float ge3, float ge4, float ge5, float ge6)
{
  if(ge1==0 || ge2==0) {
    cout<<"At least two gates should be specified !"<<endl;
    return "error";
  }
  int npad=0;
  TH1F *ha[6];
  double ge[6]={ge1,ge2,ge3,ge4,ge5,ge6};
  for(int i=0;i<6;i++) 
    if(ge[i]>1) npad++;
  
  TString sctitle=Form("gadd(%.1f",ge[0]);
  for(int i=1;i<6;i++) 
    if(ge[i]>0) sctitle+=Form(",%.1f",ge[i]);
  sctitle+=")";
  newcanvas(npad+1,sctitle);
  TString stitle="Add gates of";
  
  for(int i=0;i<npad;i++) {
    TString hname=g(ge[i],i+1);//return name of histogram.
    ha[i]=(TH1F*)gROOT->FindObject(hname);
    stitle=Form("%s %.1f",stitle.Data(),ge[i]);
  }
  stitle=Form("%s keV",stitle.Data());
  TString sname=GetHistName("gadd");    
  TH1F *hadd=(TH1F*)hg2xy->ProjectionX(sname.Data(),xmin,xmax);//copy hist structure
  hadd->SetTitle(stitle);
  hadd->Reset("M");//!!! reset maximum and minimum;

  for(int i=0;i<npad;i++) hadd->Add(hadd,ha[i]);
   if(!tlhist->FindObject(hadd))tlhist->Add(hadd);
  ca[ic]->cd(npad+1);
  icy=npad+1;
  peaks(hadd);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void gsubxr(float ge1, float ge2, float ge3, float ge4)
{
  int x1=st.xmin;
  int x2=st.xmax;
  setxrange(xmin,xmax);
  gsub(ge1, ge2, ge3, ge4);
  st.xmin=x1;
  st.xmax=x2;  
}
void gsub(float ge1, float ge2, float ge3, float ge4)
{

  if(ge1==0 || ge2==0) {
    cout<<"At least two gates should be specified !"<<endl;
    return "error";
  }
  int npad=0;
  TH1F *ha[4];
  double ge[4]={ge1,ge2,ge3,ge4};
  for(int i=0;i<4;i++) 
    if(ge[i]>1) npad++;
  
  TString sctitle=Form("gsub(%.1f",ge[0]);
  for(int i=1;i<4;i++) 
    if(ge[i]>0) sctitle+=Form(",%.1f",ge[i]);
  sctitle+=")";
  
  newcanvas(npad+1,sctitle);
  TString stitle="Sub gates of";
  for(int i=0;i<npad;i++) {
    TString hname=g(ge[i],i+1);//return name of histogram.
    ha[i]=(TH1F*)gROOT->FindObject(hname);
    stitle=Form("%s %.1f",stitle.Data(),ge[i]);
  }
  stitle=Form("%s keV",stitle.Data());
  TString sname=GetHistName("gsub");    
  TH1F *hsub=(TH1F*)hg2xy->ProjectionX(sname.Data(),xmin,xmax);//copy hist structure

  hsub->SetTitle(stitle);
  hsub->Reset("M");//!!! reset maximum and minimum;

  for(int i=1;i<npad;i++) hsub->Add(ha[0],ha[i],1,-1);
  if(!tlhist->FindObject(hsub)) tlhist->Add(hsub);
  ca[ic]->cd(npad+1);
  icy=npad+1;
  peaks(hsub);
}

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
  cout<<RED<<"Current parameter settings:"<<endl;
  cout<<CYAN<<"ncanvas = "<<st.ncanvas<<endl;
  cout<<CYAN<<"xmin = "<<st.xmin<<", xmax= "<<st.xmax<<endl;
  cout<<CYAN<<"npeaks = "<<st.npeaks<<endl;
  cout<<CYAN<<"dge1 = "<<st.dge1<<", dge2= "<<st.dge2<<endl;
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
  cout<< CYAN<< "   setxrange(int xmin1, int xmax1)        - Change range of x-axis."<<endl;
  cout<< "   setnpeaks(int npeaks1=30)              - Change the number of peaks marked."<<endl;
  cout<< "   setncanvas(int ncanvas=5)              - Change the maximum number of canvas avaliable."<<endl;
  cout<< "   setgatewidth( int dgea=-2, int dgeb=2) - Change range of gate to peak-dgea to peak+dgea."<<endl;
  cout<< "   showSettings()                         - Show current parameter settings."<<endl;
  
  cout<<YELLOW<<"....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......"<<endl;
  cout<<RED<<"Show a gated-spectrum."<<endl;
  cout<<CYAN<<"   gs(float ge1,int npad=4)"<<endl;
  cout<<RED<<"Show gated-spectra"<<endl;
  cout<<CYAN<<  "   gm(float ge1, [float ge2], [float ge3], [float ge4])"<<endl;
  cout<<CYAN<< "   gm(TH1* h1, [TH1* h2],[TH1* h3],[TH1* h4])"<<endl;
  cout<<RED<< "Show total projection sepctrum."<<endl;
  cout<<CYAN<< "   tpjm(int npad)"<<endl;
  cout<<RED<< "Show a spectrum gated from ge1 to ge2"<<endl;
  cout<<CYAN<< "   gw(float ge1,float ge2)"<<endl;
  cout<<RED<< "Show AND, ADD, SUB gated spectra"<<endl;
  cout<<CYAN<<"   gand(float ge1,float ge2, [float ge3], [float ge4])"<<endl;
  cout<<"   gadd(float ge1,float ge2, [float ge3], [float ge4], [float ge5], [float ge6])"<<endl;
  cout<<"   gsub(float ge1,float ge2, [float ge3],[float ge4])"<<endl;

  cout<<RED<<"Show gated spectra with specified the x-axis range."<<endl;
  cout<<CYAN<<"   gsxr(int xmin,int xmax,float ge1,int npad=4)"<<endl;
  cout<<CYAN<<  "   gmxr(int xmin,int xmax,double ge1, [double ge2], ...)"<<endl;
  cout<<CYAN<<  "   gmxr(int xmin,int xmax,TH1* h1, [TH1* h2], ...)"<<endl;
  cout<<CYAN<< "   tpjmxr(int xmin,int xmax,int npad)"<<endl;
  cout<<CYAN<< "   gwxr(int xmin,int xmax,float ge1,float ge2)"<<endl;
  cout<<CYAN<<"   gandxr(int xmin,int xmax,float ge1,float ge2, [float ge3], [float ge4])"<<endl;
  cout<<"   gaddxr(int xmin,int xmax,float ge1,float ge2, [float ge3], [float ge4], [float ge5], [float ge6])"<<endl;
  cout<<"   gsubxr(int xmin,int xmax,float ge1,float ge2, [float ge3],[float ge4])"<<endl; 


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
  TH1F *h=(TH1F*)hg2xy->ProjectionX(sname.Data(),xmin,xmax);
  h->SetTitle(stitle.Data());
  if(!tlhist->FindObject(h)) tlhist->Add(h);
  peaks(h);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
    
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

  int gea=hg2xy->GetXaxis()->FindBin(ge+st.dge1);
  int geb=hg2xy->GetXaxis()->FindBin(ge+st.dge2);
  if(gea<xmin) gea=xmin;
  if(geb>xmax) geb=xmax;
  TString stitle=Form("Gated on %.1f keV (%.1f - %.1f)",ge,ge+st.dge1,ge+st.dge2);
  
  TString sname=GetHistName(Form("g%d",int(ge)));
 
  TH1F *ha=(TH1F*)hg2xy->ProjectionX(sname.Data(),gea,geb);
  ha->SetTitle(stitle);
  if(!tlhist->FindObject(ha)) tlhist->Add(ha);
  peaks(ha);
  return sname;
  
}

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
  TSpectrum *s=new TSpectrum(500);
  /*
  if(st.peak_background) {
    for(int i=1;i<=h->GetNbinsX();i++) //set bins with negative counts to zero
       if(h->GetBinContent(i)<0)  h->SetBinContent(i,0);
      TH1F *hb=(TH1F*)s->Background(h,5,"same");
      h->Add(h,hb,1,-1);
      }*/
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
 
