#define excl_ana_cxx
#include "excl_ana.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <iterator>

void excl_ana::Loop()
{
//   In a ROOT session, you can do:
//      root> .L excl_ana_sim.C
//      root> excl_ana_sim t
//      root> t.GetEntry(12); // Fill t data members with entry number 12
//      root> t.Show();       // Show values of entry 12
//      root> t.Show(16);     // Read and show values of entry 16
//      root> t.Loop();       // Loop on all entries
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
  if (fChain == 0) return;

  std::cout<<"processing...\n";
  std::cout.fill('.');
  std::cout<<"# trees to be processed: "<<fChain->GetNtrees()<<std::endl;
  Long64_t nentries = fChain->GetEntries();

  Long64_t nbytes = 0, nb = 0;
  for (Long64_t jentry = 0; jentry<nentries;jentry++) {

    Long64_t ientry = LoadTree(jentry);
    
    if (ientry < 0) break;
    nb = fChain->GetEntry(jentry);
    nbytes += nb;
    
    if (!(npart == 1)) continue;
    
    if ( !(CutOpt()) ) continue;
    
    DATA[0] = revent;
    int cnt = 1;
    for (int k = 0; k<npart; k++){
      for (auto& x: brv){
	TString bn = x.first;
	x.second->GetNdata();
	DATA[cnt++] = x.second->EvalInstance(k);
      }
    }
    
    otree->Fill();
    for (int k = 0;k<cnt;k++) DATA[k] = -1111;
    std::cout<<std::setw(15)<<float(jentry+1)/nentries*100<<" %"<<"\r";
    std::cout.flush(); 
  }
  ofile->Write("",TObject::kOverwrite);
  ofile->Close();
  bm->Show("main");

}

Bool_t excl_ana::ePID()
{
  return  (vze<20) &&(-2.5<e_chi2pid&&e_chi2pid<2.5);
}


Bool_t excl_ana::DIS()
{
  return (Q2>1)&&(W>2)&&(y<0.85);
}

Bool_t excl_ana::eFID_ec()
{
  return (20<e_pcal_lv)&&(20<e_pcal_lw);
}

Bool_t excl_ana::piFID_ec(int k)
{
  return (20<det_pcal_lv[k][0])&&(20<det_pcal_lw[k][0])&&(20<det_pcal_lv[k][1])&&(20<det_pcal_lw[k][1]);
}

Bool_t excl_ana::eFID_dc()
{
  return true;
}

Bool_t excl_ana::pipFID_dc(int k)
{
  return true;
}

Bool_t excl_ana::pimFID_dc(int k)
{
  return true;
}

Bool_t excl_ana::FWD(int k)
{
  return ( ((int)det_statPart[k][0]%4000)/2000 >= 1 ) && ( ((int)det_statPart[k][1]%4000)/2000 >= 1 );
}

Bool_t excl_ana::CF(int k)
{
  return (xFm0[k]>0) && (xFm1[k]>0) && (pdata_e[k][0]/Nu>0.1) && (pdata_e[k][1]/Nu>0.1) && ((pdata_e[k][0] + pdata_e[k][1])/Nu<0.95);

}

Int_t excl_ana::LoadElecFIDPar(){
  std::ifstream fpar("/home/orsosa/Dropbox/INFN_work/Phys_ana/PID/DC_elec_par.txt");

  char junk[100];
  fpar.getline(junk,100);
  fpar.getline(junk,100);
  fpar.getline(junk,100);
  fpar.getline(junk,100);
  fpar.getline(junk,100);
  fpar.getline(junk,100);

  for (int k=0;k<NSECTORS;k++)
  {
    fpar>>pl0_e[k]>>pl1_e[k]>>pl2_e[k]>>pl3_e[k]>>pr0_e[k]>>pr1_e[k]>>pr2_e[k]>>pr3_e[k];
  }
  fpar.close();
  return 0;
}

Int_t excl_ana::setStyle(){

  myStyle  = new TStyle("orsosaStyle","My Root Styles");
  myStyle->SetHistMinimumZero(0);
  myStyle->SetPalette(1,0);
  //myStyle->SetPalette(kBlueYellow);
  myStyle->SetCanvasBorderMode(0);
  myStyle->SetPadBorderMode(0);
  myStyle->SetPadColor(0);
  myStyle->SetCanvasColor(0);
  myStyle->SetTitleFillColor(0);
  myStyle->SetTitleBorderSize(0);

  myStyle->SetStatColor(0);
  myStyle->SetOptStat("e");

  myStyle->SetLabelSize(0.05,"xyz"); // size of axis value font
  myStyle->SetTitleSize(0.05,"xyz"); // size of axis title font
  myStyle->SetTitleOffset(0.75,"xyz"); // axis title offset 
  myStyle->SetTitleFont(22,"xyz"); // font option
  myStyle->SetTitleFont(22,"a"); // pad font option
  myStyle->SetLabelFont(22,"xyz");

  myStyle->SetLabelSize(0.02,"z"); // size of axis value font
  myStyle->SetLabelOffset(-0.03,"z"); // size of axis value font
  myStyle->SetTickLength(0.002,"z"); // size of axis value font
  
  // Stat and legend fonts
  myStyle->SetStatFont(22); 
  myStyle->SetLegendFont(22); 
  // hiostogram style
  myStyle->SetHistLineWidth(2);
  myStyle->SetCanvasDefH(768);
  myStyle->SetCanvasDefW(1024);

  myStyle->SetPadBottomMargin(0.1);
  myStyle->SetPadTopMargin(0.1);
  myStyle->SetPadLeftMargin(0.1);
  myStyle->SetPadRightMargin(0.075);

  myStyle->SetPadTickX(1);
  myStyle->SetPadTickY(1);

  myStyle->SetFrameBorderMode(0);
  
  myStyle->SetGridStyle(3);
  myStyle->SetGridWidth(2);
  myStyle->SetPadGridX(kTRUE);
  myStyle->SetPadGridY(kTRUE);
  
  gROOT->SetStyle("orsosaStyle"); //uncomment to set this style
  
  return 0;
}
