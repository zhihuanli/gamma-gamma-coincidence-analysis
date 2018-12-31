
### makeggmat.C
#### makeggmat.C - Make prompt g-g coincidence matrix using RADWARE approach.

#### Usage: root -l makeggmat.C

#### Input file: 
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
#### makeggmat.C:
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
