## ROOT scripts for two-fold coincidence analysis (RADWARE approch)

# gmatrix.cpp - generate matrix data
1.modifiy gmatrix.cpp according to your experimental conditions. 
 - name of input file, outputfile
 - number of bins, minimum & maximum gamma energy 
 - time & energy condition for coincidence
 
2.how to run 
- root -l gmatrix.cpp

## gg.C - gamma-gamma analysis for prompt gg symptotic matrix
```cpp
TString fname="gg46_123_200ms.root";//filename for matrix file
```cpp
### Functions
 ```cpp
void setxrange(int xmin1=0, int xmax1=1500){ xmin=xmin1; xmax=xmax1;}; //set range of x-axis
void setnpeaks(int npeaks1=30) {npeaks=npeaks1;}; //set number of peaks found in TSpecturm. 
void setpeakwidth(double dgea=-3,double dgeb=3) {dge1=dgea;dge2=dgeb;};// set gated range: ge+dgea, ge+dgeb
void newcanvas(int ncy=1);// create new canvas with ncy pads.
void tpjm();//draw totoal projection spectrum 
void tpj(int icy=1);
TString g(double ge=0,int icy1=1);//draw a gated spectrum using defalut width setting.
//gated by ge1 and ge2 for three-fold coincidence
TString g2(double ge1=0,double ge2=0, int icy1=1);
// draw a gated spectrum with specified range of ge1-ge2
TString gw(double ge1=0,double ge2=0, int icy1=1);
// And
TString gand(double ge1=0,double ge2=0,double ge3=0,double ge4=0, double ge5=0,double ge6=0);
// Sum
TString gadd(double ge1=0,double ge2=0,double ge3=0,double ge4=0,double ge5=0,double ge6=0);
// substruction
TString gsub(double ge1=0,double ge2=0,double ge3=0,double ge4,double ge5=0,double ge6=0); 
//draw multi peaks in a canvas , up to six peaks.
void gm(double ge1,double ge2=0,double ge3=0,double ge4=0,double ge5=0,double ge6=0);
//draw multi peaks in a canvas , up to six peaks.
void gm(TH1D* h1, TH1D *h2=NULL, TH1D *h3=NULL, TH1D *h4=NULL,TH1D *h5=NULL, TH1D *h6=NULL);
TString peaks(TH1 *h, Double_t thres=0.05);
//show name of all histograms
void show(){tall->ls();};
```
#### usage:
 ```cpp
1. root -l gg.C  -- total projection spectrum will show up.
  0 1 gtpj0  [TpjPeak]
  0 1 gtpj1  [TpjPeak]
2.[ROOT] newcanvas() //create a canvas with (1x1)
3.[ROOT] g(132)      //draw gate_132 to the canvas
// Draw two gated spectra to a new canvas.
4.[ROOT] newcanvas(2) //create a cavas with （1，2）pads
5.[ROOT] g(132,1)     //draw gate_132 to 1st. pad of the canvas
6.[ROOT] g(152,2)     //draw gate_152 to 2nd. pad of the canvas
// Draw a gated spectrum to current canvas.
7.[ROOT] gw(122,125) //gate range 122-125, gw(122,125,1)
8.[ROOT] gm(123,234,567,897); -- draw 4 gated spectra on a canvas.
  1 1 g123_2  [gated on ge=123.0]
  1 2 g245_3  [gated on ge=245.0]
  1 3 g567_4  [gated on ge=567.0]
  1 4 g567_4  [gated on ge=879.0]
9.[ROOT] gand(123,245,567,867);
  2 1 g123_5  [gated on ge=123.0]
  2 2 g245_6  [gated on ge=245.0]
  2 3 g567_7  [gated on ge=567.0]
  2 4 gand0   [And gate of 123.0 245.0 567.0]
10.[ROOT] gm(g123_5,g,g245_6,gand0)
  3 3 g123_2  [gated on ge=123.0]
  3 3 g245_3  [gated on ge=245.0]
  3 3 gand0  [And gate of 123.0 245.0 567.0]
12.g2(123,245,1) - draw a spectrum gated by 123 and 245 for three-fold coincidence
  0 1 gtrip0  [two-fold gates by 123.0 keV and 245.0 keV]
//show all existing gated spectra
13.show()
OBJ: TList	TList	Doubly linked list : 0
 OBJ: TH1D	g121_2	gated on 121.0 keV : 0 at: 0x7fd6d7cb9990
 OBJ: TH1D	g244_3	gated on 244.0 keV : 0 at: 0x7fd6db11cff0
 OBJ: TH1D	gsub0	sub gate of 121.0 244.0 keV : 0 at: 0x7fd6da5f6250
 OBJ: TH1D	g121_4	gated on 121.0 keV : 0 at: 0x7fd6db121f70
 OBJ: TH1D	g121_5	gated on 121.0 keV : 0 at: 0x7fd6db246b90
14.gm(g121_2,gsub0,g244_3) - with name of histograms

// functions for setting change, all of these settings will take effect for the next drawing. 
15.[ROOT] setxrange(0,2000);//x range of spectrum
16.[ROOT] setnpeaks(30); // number of peaks marked in the spectrum
17.[ROOT] setpeakwidth(-3,3); // gate width: ge-3, ge+3

```
## ggm.C - gamma-gamma analysis for prompt gg symptotic matrix


