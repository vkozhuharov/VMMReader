#ifndef _ANALYSER_H_
#define _ANALYSER_H_

#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"

#include "SRSData.hh"
#define MAX_N_VMMS   16
#define NCHANNELS_PER_VMM 64

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
  //  TH1F *hChCharge[MAX_N_VMMS*NCHANNELS_PER_VMM];
  TH1F *hChCharge[384];
  
  int chmap[MAX_N_VMMS][NCHANNELS_PER_VMM];

};


#endif
