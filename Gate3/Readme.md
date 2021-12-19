
# gamma-gamma-gamma 三重符合分析程序 Gate3

- 程序实现了Radware的三重符合开窗谱分析功能，并在标记峰位和显示方式上有所加强。
- 程序为ROOT脚本(script)，需在ROOT6环境上运行。
- 使用者需要对ROOT的TH1，TH2功能有一定的理解。

### 生成Gate3所需二维矩阵的示例程序 

- 1.[r2g3hist.C : Make gg-matrix(TH2) from a ROOT file(TTree)](https://github.com/zhihuanli/gamma-gamma-coincidence-analysis/blob/master/Gate3/r2g3hist.C)
   - 编辑程序内的相关参数，保存后运行。
   - 方法1： 
   ```cpp
    root -l r2g3hist.C
    ```
   - 方法2：编译成.so文件运行，速度比方法1有显著提升。
   ```cpp
    root [0] .L r2g3hist.C++                 //编译生成r2g3hist_C.so
    root [1] gSystem->Load("r2g3hist_C.so") //载入内存
    root [1] r3ghist()                      //运行
   ```
- 2.[gate3bkg.C: Generate the background spectrum for gate3.C](https://github.com/zhihuanli/gamma-gamma-coincidence-analysis/blob/master/Gate3/gate3bkg.C)

### Gate3 使用方法

- 编辑gate3set.txt
```cpp
g3File      g3hist.root      // 输入文件，由r2g3hist.C 生成
prefixHist  h3xy             // 二维矩阵前缀， h3xy0010 etc., 
g3BkgFile   g3histbkg.root   // 输入本底文件， 由g3bkghist.C 生成
xyHist      h3xy             //g3Bkgfile内二维投影谱
xHist       h3x              //g3bkgfile内一维投影谱
xBkgHist    h3xb             //g3bkgfile内一维谱
xmin        0                // 最小显示范围， 在程序内用setxrange(xmin,xmax)修改
xmax        2000             // 最大显示范围
npeaks      30               // 标记gamma峰位的最大数目， 在程序内用setnpeaks(npeaks)修改
dge         2                // 开窗的范围 ge-dge到 ge+dge， 在程序内用setpeakwidth(npeaks)修改
```
- 在命令行运行
```cpp
root -l gate3.C
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
//Show gated-spectra
   gm(float ge0a, float ge0b, [float ge1a, float ge1b,float ge2a, float ge2b,float ge3a, float ge3b] )
   gm(TH1* h1, [TH1* h2],[TH1* h3],[TH1* h4])
//Show total projection sepctrum.
   tpjm(int npad)
//Show gated spectra with specified the x-axis range.
   gmxr(int xmin,int xmax,float ge0a, float ge0b, [float ge1a, float ge1b,float ge2a, float ge2b,float ge3a, float ge3b])
   gmxr(int xmin,int xmax,TH1* h1, [TH1* h2], ...)
   tpjmxr(int xmin,int xmax,int npad)
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
