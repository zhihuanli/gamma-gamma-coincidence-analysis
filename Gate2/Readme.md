
# gamma-gamma两重符合分析程序 Gate2

- [gate2.C](https://github.com/zhihuanli/gamma-gamma-coincidence-analysis/blob/master/Gate2/gate2.C) - [tutorial](https://github.com/zhihuanli/gamma-gamma-coincidence-analysis/blob/master/Gate2/gate2.ipynb)
- 程序实现了Radware的开窗谱分析功能，并在标记峰位和显示方式上有所加强。
- 程序为ROOT脚本(script)，需在ROOT6环境上运行。
- 使用者需要对ROOT的TH1，TH2功能有一定的理解。

### 生成Gate2所需二维矩阵的示例程序 

- 1.[r2ghist.C](https://github.com/zhihuanli/gamma-gamma-coincidence-analysis/blob/master/Gate2/r2g2hist.C)
   - 功能：程序将两重或两重以上的gamma关联事件保存成二维对称gamma-gamma矩阵
   - 输入文件：需要将gamma探测器相关信息以hit格式保存到ROOT文件
   - 编辑程序内的相关参数，保存后运行。
   ```cpp
   //range of runid
   int RunIdStart = 5;
   int RunIdStop = 78;
   //input
   TString fNameFormat = "./data/test%04d.root";//Path+filename, %04d: xxx0005.root for runid=5;
   TString treeName="t";
   //gamma Tree Branches in the input file
   TString ghitBranch="nGammaOneEventAddBack";//hit
   TString geBranch="nGammaEnergyAddBack";//ge[hit]
   TString gtBranch="nGammaTimeAddBack";//gt[hit]

   float dgt=18;//coincidence time window.
   //output
   int nBin = 4000;
   int MaxE = 4000;
   TString fOutName="g2hist.root";
   ```
   - 运行方法1： 
   ```cpp
    root -l r2ghist.C
    ```
   - 运行方法2：编译成.so文件运行。相比于方法1，运行速度有显著提升。
   ```cpp
    root [0] .L r2ghist.C++                 //编译生成r2g2hist_C.so
    root [1] gSystem->Load("r2g2hist_C.so") //载入内存
    root [1] r2ghist()                      //运行
   ```
    - 生成r2g2hist_C.so
- 2.[gate2matrix.C](https://github.com/zhihuanli/gamma-gamma-coincidence-analysis/blob/master/Gate2/gate2matrix.C)
  - 功能：从上述二维矩阵，生成减本底的gamma-gamma矩阵
  - 修改参数，保存后运行
  ```cpp
  //input 
   TString fInName = "g2hist.root";//root file 
   TString h2name = "h2xy";//gg-matrix 
   //output 
   TString fOutName = "gate2matrix.root";//root file
  ```

### Gate2 使用方法

- 编辑gate2set.txt
```cpp
rootfile    gate2matrix.root  //输入文件
histname    hg2xyp           //二维矩阵名称
ncanvas     5                // 最大显示窗口(TCanvas)的数目，一般不用修改， 在程序内用setncanvas(ncanvas)修改
xmin        0                // 最小显示范围， 在程序内用setxrange(xmin,xmax)修改
xmax        2000             // 最大显示范围
npeaks      30               // 标记gamma峰位的最大数目， 在程序内用setnpeaks(npeaks)修改
dge         2                // 开窗的范围 ge-dge到 ge+dge， 在程序内用setpeakwidth(npeaks)修改
```
- 在命令行运行
```cpp
root -l gate2.C
```
- 程序自动检测二维矩阵的大小
- 用help() 查看常用命令
```cpp
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//Setup, changes will take effect for the next drawing.
   setxrange(int xmin1, int xmax1)        //- Change the viewing range of x-axis for all histograms.
   setnpeaks(int npeaks1=30)              //- Change the number of peaks marked in a histogram.
   setncanvas(int ncanvas=5)              //- Change the maximum number of canvas avaliable.
   setgatewidth(int dge=2)                //- Change range of gate to peak-dge to peak+dge.
   showSettings()                         //- Show current parameter settings.
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//Show a gated-spectrum.
   gs(float ge1,int npad=4)
//Show gated-spectra
   gm(float ge1, [float ge2], [float ge3], [float ge4])
   gm(TH1* h1, [TH1* h2],[TH1* h3],[TH1* h4])
//Show total projection sepctrum.
   tpjm(int npad)
//Show a spectrum gated from ge1 to ge2
   gw(float ge1,float ge2)
//Show AND, ADD, SUB gated spectra
   gand(float ge1,float ge2, [float ge3], [float ge4])
   gadd(float ge1,float ge2, [float ge3], [float ge4], [float ge5], [float ge6])
   gsub(float ge1,float ge2, [float ge3],[float ge4])
//Show gated spectra with specified the x-axis range.
   gsxr(int xmin,int xmax,float ge1,int npad=4)
   gmxr(int xmin,int xmax,double ge1, [double ge2], ...)
   gmxr(int xmin,int xmax,TH1* h1, [TH1* h2], ...)
   tpjmxr(int xmin,int xmax,int npad)
   gwxr(int xmin,int xmax,float ge1,float ge2)
   gandxr(int xmin,int xmax,float ge1,float ge2, [float ge3], [float ge4])
   gaddxr(int xmin,int xmax,float ge1,float ge2, [float ge3], [float ge4], [float ge5], [float ge6])
   gsubxr(int xmin,int xmax,float ge1,float ge2, [float ge3],[float ge4])
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//Save a hist to a ROOT file with append mode
    saveHist(TH1* hist, TString sfile)
//Save a canvas to a ROOT file with append mode
    saveCanv(TCanvas* c1, TString sfile)
//Save a hist data to a text file
    saveHistData(TH1* c1, TString sfile)
//Save All histograms to a ROOT file
    saveHistAll(TString sfile)
//Save All TCanvas to a ROOT file
    saveCanvAll(TString sfile)
//Show all existing gated histograms
   histlist()
//Show all existing TCanvas
   canvlist()
//Safe way to quit the program in mac.
   quit()
```
