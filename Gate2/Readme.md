

### gamma-gamma两重符合分析程序 Gate2

- 设ROOT文件名为 file.root, 二维 𝛾−𝛾 矩阵的名称为hg2 (TH2* hg2).
- 在终端命令行运行
```cpp
root -l 'gate2.C("file.root","hg2")'
```
进入ROOT后在ROOT命令行运行
```cpp
.x gate2.C("file.root","hg2")
```
### 生成Gate2所需的二维矩阵的示例程序
- 1.Generate gamma-gamma matrix(TH2) from a ROOT file(TTree)
  <https://github.com/zhihuanli/gamma-gamma-coincidence-analysis/blob/master/Gate2/root2hist.html>
- 2.Generate background subtructed gamma-gamma matrix using Radware approch
  <https://github.com/zhihuanli/gamma-gamma-coincidence-analysis/blob/master/Gate2/hist2radware.html>

