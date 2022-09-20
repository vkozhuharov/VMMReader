#include "Analyser.hh"
#include <iostream>
#include "TCanvas.h"

void Analyser::init(){
  for(int i = 0;i<2;i++){
    for(int j=0;j<64;j++){
      chmap[i][j] = -1;
    }
  }
  chmap[1][63] = 0;
  chmap[1][62] = 1;
  chmap[1][59] = 2;
  chmap[1][58] = 3;
  chmap[1][55] = 4;
  chmap[1][54] = 5;
  chmap[1][51] = 6;
  chmap[1][50] = 7;
  chmap[1][47] = 8;
  chmap[1][46] = 9;
  chmap[1][43] = 10;
  chmap[1][42] = 11;
  chmap[1][39] = 12;
  chmap[1][38] = 13;
  chmap[1][35] = 14;
  chmap[1][34] = 15;
  chmap[1][31] = 16;
  chmap[1][30] = 17;
  chmap[1][27] = 18;
  chmap[1][26] = 19;
  chmap[1][23] = 20;
  chmap[1][22] = 21;
  chmap[1][19] = 22;
  chmap[1][18] = 23;
  chmap[1][15] = 24;
  chmap[1][14] = 25;
  // chmap[1][7] = 26;
  // chmap[1][6] = 27;
  // chmap[1][3] = 28;
  // chmap[1][2] = 29;

  chmap[1][11] = 26;
  chmap[1][10] = 27;
  chmap[1][7] = 28;
  chmap[1][6] = 29;
  chmap[1][3] = 30;
  chmap[1][2] = 31;

  
  // chmap[0][63] = 30;
  // chmap[0][62] = 31;
  // chmap[0][59] = 32;
  // chmap[0][58] = 33;
  // chmap[0][55] = 34;
  // chmap[0][54] = 35;
  // chmap[0][51] = 36;
  // chmap[0][50] = 37;
  // chmap[0][47] = 38;
  // chmap[0][46] = 39;
  // chmap[0][43] = 40;
  // chmap[0][42] = 41;
  // chmap[0][39] = 42;
  // chmap[0][38] = 43;
  // chmap[0][35] = 44;
  // chmap[0][34] = 45;
  // chmap[0][31] = 46;
  // chmap[0][30] = 47;

  chmap[0][63] = 32;
  chmap[0][62] = 33;
  chmap[0][59] = 34;
  chmap[0][58] = 35;
  chmap[0][55] = 36;
  chmap[0][54] = 37;
  chmap[0][51] = 38;
  chmap[0][50] = 39;
  chmap[0][47] = 40;
  chmap[0][46] = 41;
  chmap[0][43] = 42;
  chmap[0][42] = 43;
  chmap[0][39] = 44;
  chmap[0][38] = 45;
  chmap[0][35] = 46;
  chmap[0][34] = 47;
  // chmap[0][31] = 48;
  // chmap[0][30] = 49;

 
}


Analyser::Analyser (){
  fout = new TFile("output.root","RECREATE");

  hChIndex = new TH1F("hCh","Channel index",1024,0.0,1024.0);
  hChNumber = new TH1F("hChNumber","Channel number",1024,0.0,1024.0);
  hNHits = new TH1F("hNHits","Number of hits",1024,0.0,1024.0);
  hChOccupancy = new TH2F("hChOccupancy","Channel occupancy",32,0,32,32,0,32);
  hEventChOccupancy = new TH2F("hEventChOccupancy","Channel occupancy",32,0,32,32,0,32);

  hChChargeOccupancy = new TH2F("hChChargeOccupancy","Cumulative charge",32,0,32,32,0,32);
  hEventChChargeOccupancy = new TH2F("hEventChChargeOccupancy","Cumulative event charge",32,0,32,32,0,32);

  hCharge = new TH1F("hCharge","Charge distribution",1000,0.0,1000.0);
  
  
  init();
  
  
}


Analyser::~Analyser(){
  //write the histograms and the file and exit

  fout->Write();
  fout->Close();
  
}

void Analyser::process(SRSData *srs){
  static int iev;

  int nhits = srs->getHits().size();
  hNHits->Fill(nhits);

  hEventChOccupancy->Reset();
  hEventChChargeOccupancy->Reset();

  for(int ih = 0;ih < nhits;ih++) {

    
    std::cout << "Hit Channel number:  " <<64.*srs->hits[ih]->vmmid +  1.*srs->hits[ih]->chno << std::endl;
    hChIndex->Fill(64.*srs->hits[ih]->vmmid +  1.*srs->hits[ih]->chno);

    
    std::cout << "VMM id: " << 1.*srs->hits[ih]->vmmid  << std::endl;

    //    if (srs->hits[ih]->vmmid > 1 ) {
    if (srs->hits[ih]->vmmid != 2  &&
	srs->hits[ih]->vmmid != 3  &&
	srs->hits[ih]->vmmid != 6  &&
	srs->hits[ih]->vmmid != 7  &&
	srs->hits[ih]->vmmid != 10 &&
	srs->hits[ih]->vmmid != 11
	) {
      std::cout << "Unknown VMM chip: " << 1.*srs->hits[ih]->vmmid  << std::endl;
      continue;
    }

    if (srs->hits[ih]->chno > 63 ) {
      std::cout << "Unknown channel: " << 1.*srs->hits[ih]->chno  << std::endl;
      continue;
    }

    int ch = chmap[srs->hits[ih]->vmmid][srs->hits[ih]->chno];

    if (ch == -1) {
      //may be for some noise estimations
      std::cout << "Hit from unconnected channel: " << ch << std::endl;
      continue;
    }
    hChNumber->Fill(ch);
    hCharge->Fill(1.*srs->hits[ih]->adc);
    

    int ix = ch/6;
    int iy = ch%6;

    hChOccupancy->Fill(ix,iy);
    hEventChOccupancy->Fill(ix,iy);
    
    hChChargeOccupancy->Fill(ix,iy,1.*srs->hits[ih]->adc);
    hEventChChargeOccupancy->Fill(ix,iy,1.*srs->hits[ih]->adc);
    
  
  }

  char fname[64];

  TCanvas c1;
  c1.cd();
  
  hEventChOccupancy->Draw("colz");
  sprintf(fname,"pics/plot-occ%04d.png",iev);
  c1.Print(fname);

  hEventChChargeOccupancy->Draw("colz");
  sprintf(fname,"pics/plot-charge%04d.png",iev);
  c1.Print(fname);

  
  iev ++;
  
}
