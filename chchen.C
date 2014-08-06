#define chchen_cxx
#include "chchen.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <string>
void chchen::Loop()
{
//   In a ROOT session, you can do:
//      Root > .L chchen.C
//      Root > chchen t
//      Root > t.GetEntry(12); // Fill t data members with entry number 12
//      Root > t.Show();       // Show values of entry 12
//      Root > t.Show(16);     // Read and show values of entry 16
//      Root > t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch
  
//declear histogram and variable
  std::string title[2]={"Barrel","Endcap"};
  std::string subtitle[2]={"Layer","Disk"};
  TH1F* hcount_1d = new TH1F("hcount_1d","",5, 0.5,5.5);
  TH1F* htof = new TH1F("htof","",100,-0.5,0.5);
  TH1F* ftof = new TH1F("ftof","",100,0.0,20.0);
  TH1F* h[2][15];
  TH1F* ht[2][15];
  TH1F* f[2][15];
  for(int k=0;k<2; k++){
    for(int i=0;i<15;i++){
      
      h[k][i]=(TH1F*)hcount_1d->Clone(Form("h%d%02i",k,i));
      h[k][i]->SetXTitle("Number of crossings per particle");
      h[k][i]->SetTitle(Form("%s, %s %d",title[k].data(),
			       subtitle[k].data(),i+1));

      ht[k][i]=(TH1F*)htof->Clone(Form("ht%d%02i",k,i));
      ht[k][i]->SetXTitle("Difference of TOF from the first hit: ns");
      ht[k][i]->SetTitle(Form("%s, %s %d",title[k].data(),
			       subtitle[k].data(),i+1));

      f[k][i]=(TH1F*)ftof->Clone(Form("f%d%02i",k,i));
      f[k][i]->SetXTitle("Tine of Flight Distrubution");
      f[k][i]->SetTitle(Form("%s, %s %d",title[k].data(),
			       subtitle[k].data(),i+1));

    }
  }

  TH2F* hcount = new TH2F("hcount","",15,0.5,15.5,5, 0.5,5.5);
  TH2F* hbarrel = (TH2F*)hcount->Clone("hbarrel");
  hbarrel->SetTitle("Barrel");
  hbarrel->SetXTitle("Layer");
  hbarrel->SetYTitle("Number of crossings per particle");
  TH2F* hendcap = (TH2F*)hcount->Clone("hendcap");
  hendcap->SetTitle("Endcaps");
  hendcap->SetXTitle("Disk");
  hendcap->SetYTitle("Number of crossings per particle");

  int countB[10]={0};
  int countE[15]={0};
  float tofB[10]={0.};
  float tofE[15]={0.};

  if (fChain == 0) return;
  
  Long64_t nentries = fChain->GetEntriesFast();

  Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;
      
      for(int i=0;i<10;i++)countB[i]=0;
      for(int i=0;i<15;i++)countE[i]=0;
      
      for(int i=0; i < hitSubDec->size(); i++){

	if(hitPID->at(i)!= +13)continue;
	int hitLayerIndex = hitLayer->at(i)-1;
	int hitDiskIndex = hitDisk->at(i)-1;
	if(hitSubDec->at(i)==1)
	  {
	    countB[hitLayerIndex]++;
	    if(countB[hitLayerIndex]==1){
	      tofB[hitLayerIndex]= hitTof->at(i);
              f[0][hitLayerIndex]->Fill(tofB[hitLayerIndex]);
                  }
	    else if(countB[hitLayerIndex]>1)
	      ht[0][hitLayerIndex]->Fill(hitTof->at(i)-tofB[hitLayerIndex]);
	  }
	else
	  if(hitSubDec->at(i)==2)
	    {
	      countE[hitDiskIndex]++;
	      if(countE[hitDiskIndex]==1){
		tofE[hitDiskIndex]= hitTof->at(i);
                f[1][hitDiskIndex]->Fill(tofE[hitDiskIndex]);
                 }
	      else if(countE[hitDiskIndex]>1)
		ht[1][hitDiskIndex]->Fill(hitTof->at(i)-tofE[hitDiskIndex]);
	    }
      }
   
      
      for(int i=0;i<10;i++){
	hbarrel->Fill(i+1,countB[i]);
	h[0][i]->Fill(countB[i]);
      }
      for(int i=0;i<15;i++){
	hendcap->Fill(i+1,countE[i]);
	h[1][i]->Fill(countE[i]);
      }
      
      
   }

   //hbarrel->Draw();
   //hendcap->Draw();
   TFile* outFile = new TFile("histo.root","recreate");       
   
   hbarrel->Write();
   hendcap->Write();
   for(int i=0;i<2;i++){
     for(int j=0; j<15;j++)
       {
	 h[i][j]->Write();
	 ht[i][j]->Write();
         f[i][j]->Write();
    
       }
   }

  outFile->Close();
}
