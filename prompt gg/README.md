## new code for two-fold, three-fold coincidence analysis
# gmatrix.C
---
## ROOT data format
### Branch in gammaX.root files
- ahit: number of hit in a event (after addback for Clover);
- aid[ahit]:detector id
- ae[ahit]: energy
- at[ahit]: time

### Procedures to create two-fold, tree-fold matrix data
1.Create gmatix.C
- root -l
- root [0] TChain ch("tree");
- root [1] ch.Add("gamma1.root");//Add all your gamma files to ch
- root [2] ch.Add("gamma2.root");
- ... ...
- root [9] ch.MakeClass("gmatix"); //create gmatrix.C & gmatrix.h
- root [10].q

2.modifiy gmatrix.C according to your experimental conditions. Refer to gmatrix.C code.
 - name of input file, outputfile
 - number of bins, minimum & maximum gamma energy 
 - time & energy condition for coincidence

3.make matrix data gamma.root
- root -l
- root [0] .L gmatrix.C
- root [1] gmatrix t
- root [2] t.Loop();
- root [3] .q

### test code: gg2.C, ggg2.C 


## old code for two-fold analysis
--- 
## makeggmat.C
#### makeggmat.C - Make prompt g-g coincidence matrix using RADWARE approach.
#### Usage: root -l makeggmat.C
 
### makeggmat.C:
Following lines in the code should be modified by user

 ```cpp
 TString rootfile="../../decay46_123_all.root"; // Name of input file
 TString ggfile="43_123gg.root"; // Name of output file
 ......
 //condition for gg matrix 
 TString scut0="abs(caxt-cayt)<200&&decaytime<200"; 
 //make gg 2D histogram 
 tree->Draw("caxe:caye>>gg(4096,0,4096,4096,0,4096)",scut0,"colz");
```
---
## gg.C - gamma-gamma analysis for prompt gg symptotic matix
#### original parameter setting 
 ```cpp
//range of histogram, it can be modified by setxrange(xmin,xmax); 
int xmin=0;
int xmax=3000;
//number of marked peaks in the spectrum, it can be modified by setnpeaks(npeaks); 
int npeaks=30;
//range of gate, gate width:ge+dge1 - ge+dge2, it can be modified by setgatewidth(dge1,dge2)
int dge1=-3;
int dge2=3;
//Name of input file
TString fname="46_123gg.root";
```
### Functions
 ```cpp
void setxrange(int xmin1=0, int xmax1=1500){ xmin=xmin1; xmax=xmax1;}; //set range of x-axis
void setnpeaks(int npeaks1=30) {npeaks=npeaks1;}; //set number of peaks found in TSpecturm. 
void setpeakwidth(double dgea=-3,double dgeb=3) {dge1=dgea;dge2=dgeb;};// set gated range: ge+dgea, ge+dgeb
void newcanvas(int ncy=1);// create new canvas with ncy pads.
void tpjm();//draw totoal projection spectrum 
void tpj(int icy=1);
TString g(double ge,int icy1=1);//draw a gated spectrum using defalut width setting.
TString gw(double ge1,double ge2, int icy1=1);// draw a gated spectrum with specified range of ge1-ge2
TString gand(double ge1,double ge2,double ge3=0,double ge4=0,
	  double ge5=0,double ge6=0);// and of all spectra ge1xge2xge3... 
TString gadd(double ge1,double ge2,double ge3=0,double ge4=0,
	  double ge5=0,double ge6=0);// sum of all spectra  ge1+ge2+ge3...
TString gsub(double ge1,double ge2,double ge3=0,double ge4=0,
	     double ge5=0,double ge6=0); // substruction of spectra: ge1-ge2-ge3...
void gm(double ge1,double ge2=0,double ge3=0,double ge4=0,
	double ge5=0,double ge6=0);//draw multi peaks in a canvas , up to six peaks.
void gm(TH1D* h1, TH1D *h2=NULL, TH1D *h3=NULL, TH1D *h4=NULL,
	TH1D *h5=NULL, TH1D *h6=NULL);//show multi-gated peaks using histograms as parameters
TString peaks(TH1 *h, Double_t thres=0.05);
void show(){tall->ls();};//show name of all histograms
```
#### usage:
 ```cpp
1. root -l ge.C  -- total projection spectrum will show up.
  0 1 gtpj0  [TpjPeak]
  0 1 gtpj1  [TpjPeak]
2.[ROOT] gm(123,234,567,897); -- draw 4 gated spectra on a canvas.
  1 1 g123_2  [gated on ge=123.0]
  1 2 g245_3  [gated on ge=245.0]
  1 3 g567_4  [gated on ge=567.0]
3.[ROOT] gand(123,245,567,867);
  2 1 g123_5  [gated on ge=123.0]
  2 2 g245_6  [gated on ge=245.0]
  2 3 g567_7  [gated on ge=567.0]
  2 4 gand0   [And gate of 123.0 245.0 567.0]
4.[ROOT] gm(g123_5,g,g245_6,gand0)
  3 3 g123_2  [gated on ge=123.0]
  3 3 g245_3  [gated on ge=245.0]
  3 3 gand0  [And gate of 123.0 245.0 567.0]
5.[ROOT] newcanvas() //create a canvas with (1x1)
6.[ROOT] g(132)      //draw gate_132 to the canvas
// Draw two gated spectra to a new canvas.
7.[ROOT] newcanvas(2) //create a cavas with （1，2）pads
8.[ROOT] g(132,1)     //draw gate_132 to 1st. pad of the canvas
9.[ROOT] g(152,2)     //draw gate_152 to 2nd. pad of the canvas

// Draw a gated spectrum to current canvas.
10.[ROOT] g(123) -- draw gate_123 to current canvas.
11.[ROOT] gw(122,125)
// functions for setting change, all of these settings will take effect for the next drawing. 
11.[ROOT] setxrange(0,2000);//x range of spectrum
12.[ROOT] setnpeaks(30); // number of peaks marked in the spectrum
13.[ROOT] setpeakwidth(-3,3); // gate width: ge-3, ge+3

```
