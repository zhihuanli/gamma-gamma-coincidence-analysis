
## makeaggmat.C
#### makeggmat.C - Make gamma -delayed gamma coincidence matrix using RADWARE approach.
#### Usage: root -l makedggmat.C
### Input file is generated from ...

### makedggmat.C:
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
## agg.C - gamma-gamma analysis for g-delayed g asymptotic matix
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
#### usage: root -l age.C 
 ```cpp
// Draw a gated spectrum to a new canvas.
newcanvas() //create a canvas with (1,1)
gx(132),gy(123)      //draw gate_132 to the canvas
// Draw two gated spectra to a new canvas.
newcanvas(2) //create a cavas with （1，2）pads
gx(132,1)     //draw gate_132 to 1st. pad of the canvas
gy(152,2)     //draw gate_152 to 2nd. pad of the canvas
// Draw a gated spectrum to current canvas.
gx(123) -- draw gate_123 to current canvas.
// Draw gated spectra to a new canvas. -- support up to six spectra
gxm(123,234,567,897); -- draw 4 gated spectra on a canvas.
gym(123,234,567,897); -- draw 4 gated spectra on a canvas.
// functions for setting change, all of these settings will take effect for the next drawing. 
setxrange(0,2000);//x range of spectrum
setnpeaks(30); // number of peaks marked in the spectrum
setpeakwidth(-3,3); // gate width: ge-3, ge+3

```
