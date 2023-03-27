#ifndef _ANALYSER_H_
#define _ANALYSER_H_

#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"

#include "SRSData.hh"

class Analyser
{
public:
  Analyser();
  Analyser(char *s);
  ~Analyser();
  void init();

  void ClearTestFile();
  void process(SRSData *srs);
  void plotting();
  
  
public:
  TFile *fout;
  
  TH1F *hChIndex;
  TH1F *hChNumber;
  // TH1F *hEventChNumber;
  TH1F *hNHits;
  // TH1F *hNHitsEvent;

  // TH2F *hChOccupancy;
  // TH2F *hEventChOccupancy;
  // TH2F *hChChargeOccupancy;
  // TH2F *hEventChChargeOccupancy;

  TH2F *hHitOccupancy;
  TH2F *hHitChargeOccupancy;
  TH2F *hFilteredHitChargeOccupancy;
  TH2F *hFilteredHitChargeOccupancy1;
  TH2F *hSaturationMap;
  
  TH1F *hCharge;
  TH1F *hTotalCharge;
  TH1F *hTotalCharge_Skip;
  TH1F *hTotalCharge_Hop;
  TH1F *hTotalChargeNS;
  TH1F *hTotalCharge_Sat;
  
  TH1F *hTotalChargeFilt[5];
  TH1F *hTotalChargeFilt_Skip[5];
  TH1F *hTotalChargeFilt_Hop[5];
  TH1F *hHitSplitCharge[5];
  
  // TH1F *hChCharge[64];
  // TH1F *hCharge1[64];

  TH1F *hEventSaturationPNZ;
  TH1F *hEventSaturationNZ;
  TH1F *hEventSaturationP;
  TH1F *hEventSaturation;
  TH1F *hSaturation[8];
  TH1F *hTotalSaturation;
  TH1F *hSingleSat[8];
  TH1F *hSingleSatF;
  
  int chmap[16][64];
  char name[64];
};
#endif
