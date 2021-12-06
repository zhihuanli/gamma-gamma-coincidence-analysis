

### gamma-gamma两重符合分析程序 Gate2
- 编辑gate2set.txt
```cpp
    - rootfile    hgglmatrix.root  //输入文件
    - histname    hg2xyp           //二维矩阵名称
    - ncanvas     5                // 不用修改
    - xmin        0                // 最小显示范围
    - xmax        2000             // 最大显示范围
    - npeaks      30               // 标记gamma峰位的最大数目
    - dge         2                // 开窗的范围 ge-dge到 ge+dge
```
- 程序自动检测二维矩阵的大小

### 生成Gate2所需的二维矩阵的示例程序 （下载html文件后在本机点击查看）
- 1.Generate gamma-gamma matrix(TH2) from a ROOT file(TTree)
    - <https://zhihuanli.github.io/gamma-gamma-coincidence-analysis/blob/master/Gate2/root2hist.html>
- 2.Generate background subtructed gamma-gamma matrix using Radware approch
    - <https://zhihuanli.github.io/gamma-gamma-coincidence-analysis/blob/master/Gate2/hist2radware.html>

