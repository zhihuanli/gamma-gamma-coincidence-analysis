#include "TH3.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TSystem.h"

#include <fstream>
#include <iostream>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int root2m4b(TString hfile, TString hname,TString m4bfile)
{
  TFile *file = new TFile(hfile.Data(),"READ");//"RECREATE" "READ"

  TH2I *h = (TH2I*)file->Get(hname.Data());
  h->Print();

  ofstream writefile;//fstream
  writefile.open(m4bfile.Data(),std::ios::binary);//ios::bin ios::app

  double raw;
  int data;
   for (int i = 0; i < 4096; ++i)
    for (int j = 0; j < 4096; ++j)
      {
	raw = h->GetBinContent(i+1, j+1);
	data = (int) raw;
	writefile.write((char*)&data,sizeof(int));
      }
   
  writefile.close();
  file->Close();
  return 0;
}
// 
// root2m4b.cc ends here
