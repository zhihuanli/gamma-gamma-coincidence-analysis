## gamma -delayed gamma coincidence matrix using RADWARE approach.

[1.生成delayedTree](delayedTree.ipynb)

[2.生成delayedMatrix](delayedMatrix.ipynb)

[3.delayedgg分析](delayedgg.ipynb)


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
ga(132),gb(123)      //draw gate_132 to the canvas
// Draw two gated spectra to a new canvas.
newcanvas(2) //create a cavas with （1，2）pads
ga(132,1)     //draw gate_132 to 1st. pad of the canvas
gb(152,2)     //draw gate_152 to 2nd. pad of the canvas
// Draw a gated spectrum to current canvas.
ga(123) -- draw gate_123 to current canvas.
// Draw gated spectra to a new canvas. -- support up to six spectra
gam(123,234,567,897); -- draw 4 gated spectra on a canvas.
gbm(123,234,567,897); -- draw 4 gated spectra on a canvas.
// functions for setting change, all of these settings will take effect for the next drawing. 
setxrange(0,2000);//x range of spectrum
setnpeaks(30); // number of peaks marked in the spectrum
setpeakwidth(-3,3); // gate width: ge-3, ge+3

```
