
# gamma-gamma两重符合分析程序 Gate2

- 程序为ROOT脚本(script)，在ROOT6环境上运行。
- 程序实现了Radware的开窗谱分析功能，并在标记峰位和显示方式上有所加强。
- 使用者需要对ROOT的TH1，TH2功能有一定的理解。

### 生成Gate2所需二维矩阵的示例程序 （下载html文件后在本机点击查看）

- 1.Generate gamma-gamma matrix(TH2) from a ROOT file(TTree)
  - <https://zhihuanli.github.io/gamma-gamma-coincidence-analysis/blob/master/Gate2/root2hist.html>
- 2.Generate background subtructed gamma-gamma matrix using Radware approch
  - <https://zhihuanli.github.io/gamma-gamma-coincidence-analysis/blob/master/Gate2/hist2radware.html>

### Gate2 使用方法

- 编辑gate2set.txt
```cpp
rootfile    hgglmatrix.root  //输入文件
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
