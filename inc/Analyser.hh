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
  Analyser( char *s );
  ~Analyser();
  void init();

  int elem_index( auto array, int vmm );
  void ClearTestFile();
  void process( SRSData *srs );
  void plotting();

  std::map< std::pair< int, int >, std::pair< int, int > >
  load_table( const std::string &filename );
  
  
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
  // TH1F *hTotalCharge_Skip;
  // TH1F *hTotalCharge_Hop;
  // TH1F *hTotalCharge_Hop_Test;
  TH1F *hTotalChargeNS;
  TH1F *hTotalCharge_Sat;
  
  TH1F *hTotalChargeFilt[5];
  // TH1F *hTotalChargeFilt_Skip[5];
  // TH1F *hTotalChargeFilt_Hop[5];
  // TH1F *hTotalChargeFilt_Hop_Test[5];
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
  
  int chmap[16][128];
  char name[64];

  struct Hit_Container
  {
    int vmm;
    int channel;
    int bcid;
    int charge;
  } Hop_Buffer;

  struct Packet_Events
  {
    int NumHits;
    int TotCharge;
    int Saturated;
  } Pack_Buffer;

  std::vector< Hit_Container > Hop_Event;
  std::vector< Hit_Container > Hop_Outliers;
  std::vector< Packet_Events > Packet;
};
#endif
