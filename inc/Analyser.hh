#ifndef _ANALYSER_H_
#define _ANALYSER_H_

#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"

#include "SRSData.hh"

class Analyser {
public:
  Analyser();
  ~Analyser();
  void init();
  
  void process(SRSData *srs);
  
  
public:
  TFile *fout;
  TH1F *hNHits;
  TH1F *hChIndex;
  TH1F *hChNumber;
  TH2F *hChOccupancy;
  TH2F *hEventChOccupancy;
  
  TH2F *hChChargeOccupancy;
  TH2F *hEventChChargeOccupancy;

  TH1F *hCharge;
  TH1F *hChCharge[64];
  
  int chmap[2][64];

};


#endif
