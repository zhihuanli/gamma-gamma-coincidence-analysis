### 1. Generate $\gamma-\gamma$ matrix(TH2) from a ROOT file(TTree)


```c++
int root2hist(int run)
{
    TString fname=Form("./data/test%04d.root",run);//modify by user
    if(gSystem->AccessPathName(fname.Data())){
       cout << fname.Data()<<" does not exist" << endl;
       return 0;
    } 
    TFile *f=new TFile(fname.Data());
    TTree *tree=(TTree*) f->Get("t");//user
    int ghit;
    double ge[100];
    Long64_t gt[100];
    tree->SetBranchAddress("nGammaOneEventAddBack",&ghit);//user
    tree->SetBranchAddress("nGammaEnergyAddBack",&ge);//user
    tree->SetBranchAddress("nGammaTimeAddBack",&gt);//user
    TH2F *hgg=new TH2F("hggl","hggl",4096,0,4096,4096,0,4096);//user
    Long64_t nentries=tree->GetEntries();
    cout<<run<<","<<nentries<<endl;
    for(Long64_t jentry=0;jentry<nentries;jentry++)  {
        tree->GetEntry(jentry);
        if(ghit<2) continue;
        for(int i=0;i<ghit;i++) {
            for(int j=0;j<ghit;j++) {
                if(i==j) continue;
                if(abs(gt[i]-gt[j])>18) continue; // coincidence time window
                hgg->Fill(ge[i],ge[j]);
                hgg->Fill(ge[j],ge[i]);
            }
        }
    }
    TFile *fout=new TFile(Form("hg%02d.root",run),"recreate");
    hgg->Write();
    fout->Close();
    f->Close();
    return 1;  
}
```


```c++
for(int i=5;i<78;i++)//user
    root2hist(i);
```

在终端中利用 hadd命令将所有生成的root文件合并成一个root文件 hgg.root
- hadd hgg.root hg05.root hg06.root ....
