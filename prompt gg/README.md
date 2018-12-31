
## makeggmat.C
#### makeggmat.C - Make prompt g-g coincidence matrix using RADWARE approach.
#### Usage: root -l makeggmat.C
### Input file: 
 Includes four branches of dimensional gamma-gamma: caxe[hit],caxt[hit],caye[hit],cayt[hit].
 Those two branches are generated from original gamma events (ae[idet], at[idet], idet= number of det) by the following way:
 ```cpp
 int hit=0;
 for(int i=0;i<idet;i++) {
   for(int j=0;j<idet;j++) {
      if(i==j) continue;
      if(ge[i]<10 || ge[j]<10 || gt[i]<10 || gt[j]<10) continue; //condition for gamma-gamma coincidence.
      caxe[hit]=ae[i];//energy
      caxt[hit]=at[i];//time
      caye[hit]=ae[j];
      cayt[hit]=aet[j];
      hit++;
    }
  }
 ```   
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
## gg.C - gamma-gamma analysis for prompt gg symptotic matix
#### usage: root -l ge.C 
 ```cpp
// Draw a gated spectrum to a new canvas.
newcanvas() //create a canvas with (1,1)
g(132)      //draw gate_132 to the canvas
// Draw two gated spectra to a new canvas.
newcanvas(2) //create a cavas with （1，2）pads
g(132,1)     //draw gate_132 to 1st. pad of the canvas
g(152,2)     //draw gate_152 to 2nd. pad of the canvas
// Draw a gated spectrum to current canvas.
g(123) -- draw gate_123 to current canvas.
// Draw gated spectra to a new canvas. -- support up to six spectra
gm(123,234,567,897); -- draw 4 gated spectra on a canvas.

// functions for setting change, all of these settings will take effect for the next drawing. 
setxrange(0,2000);//x range of spectrum
setnpeaks(30); // number of peaks marked in the spectrum
setpeakwidth(-3,3); // gate width: ge-3, ge+3

```
