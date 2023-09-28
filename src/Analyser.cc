#include <algorithm>
#include <iostream>
#include <typeinfo>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <string>
#include <vector>
#include <array>
#include <map>

#include "TDirectory.h"
#include "TCanvas.h"
#include "TString.h"
#include "TGraph.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TF1.h"

#include "Analyser.hh"


int cl_count = 0;
int pr_count = 0;
int prints = 0;
int fhitc = 0;
int outc = 0;
int begin_ih;
int end_ih;


struct Mapping
{
  int vmm;
  int channel;
  int c_x;
  int c_y;
  int canvas;
};


std::vector< std::vector< Mapping > > New_Map;


int Analyser::elemIndex( auto array, int vmm )
{
  for( int i = 0; i < array.size(); i++ )
  {
    // std::cout << VMMs[i] << std::endl;
    if( array[i] == vmm ) { return i; }
  }
  
  return -1;
}


std::map< std::pair< int, int >, std::pair< int, int > >
Analyser::load_table( const std::string &filename )
{
  int b, c, x, y;
  std::map< std::pair< int, int >, std::pair< int, int > > table;
  std::ifstream fdata( filename.c_str() );
  while( fdata >> b >> c >> x >> y )
    table[{ b, c }] = { x, y }; // Read the table and store the values
  
  return table;
}


std::map< std::pair< int, int >, std::array< int, 3 > >
Analyser::mapLoader( const std::string &filename )
{
  int vmm, ch, can, x, y;
  std::map< std::pair< int, int >, std::array< int, 3 > > table;
  std::ifstream fdata( filename.c_str() );
  while( fdata >> vmm >> ch >> can >> x >> y )
    table[{ vmm, ch }] = { can, x, y };   // Read the table and store the values
  
  return table;
}


void Analyser::init()
{
  for( int i = 0; i < 2; i++)
    for( int j = 0; j < 64; j++) { CH_Mapper[i][j] = -1; }
  
  for( auto HGCH: HGChannels )
    CH_Mapper[0][HGCH] = 1;

  for( auto LGCH: LGChannels )
    CH_Mapper[1][LGCH] = 1;
}


Analyser::Analyser()
{
  char hName[64];
  char hTitle[64];

  fOut = new TFile( "output.root", "RECREATE" );

  hSeparator[1] = new TH1F( "1---------------------------------------------", "", 1, 0.0, 1.0 );

  hChActivation = new TH1F( "hChActivation", "Channel activation", 1700, 0.0, 1700.0 );
  hChRepetition = new TH1F( "hChRepetition", "Channel repetition", 1700, 0.0, 1700.0 );
  hChRepetitionHG = new TH1F( "hChRepetitionHG", "Channel repetition in HG only", 1700, 0.0, 1700.0 );
  hChRepetitionLG = new TH1F( "hChRepetitionLG", "Channel repetition in LG only", 1700, 0.0, 1700.0 );
  hChRepetitionEvent = new TH1F( "hChRepetitionEvent", "Channel repetition per event", 500, 0.0, 500.0 );
  hChRepetitionEventHG = new TH1F( "hChRepetitionEventHG", "Channel repetition per event in HG", 250, 0.0, 250.0 );
  hChRepetitionEventLG = new TH1F( "hChRepetitionEventLG", "Channel repetition per event in LG", 250, 0.0, 250.0 );

  hSeparator[1] = new TH1F( "2---------------------------------------------", "", 1, 0.0, 1.0 );

  hTotalChargeHG = new TH1F( "hTotalChargeHG", "Total charge distribution", 2500, 0.0, 80000.0 );
  hTotalChargeHG_F = new TH1F( "hTotalChargeHG_F", "Total charge distribution", 2500, 0.0, 80000.0 );
  hTotalChargeLG = new TH1F( "hTotalChargeLG", "Total charge distribution", 2500, 0.0, 80000.0 );
  hTotalChargeLG_F = new TH1F( "hTotalChargeLG_F", "Total charge distribution", 2500, 0.0, 80000.0 );
  hTotalChargeCal = new TH1F( "hTotalChargeCal", "Total charge distribution after calibration", 2500, 0.0, 80000.0 );
  hTotalChargeCalHG = new TH1F( "hTotalChargeCalHG", "Total charge distribution for HG at calibration time", 2500, 0.0, 80000.0 );
  hTotalChargeCalLG = new TH1F( "hTotalChargeCalLG", "Total charge distribution for LG at calibration time", 2500, 0.0, 80000.0 );

  hSeparator[2] = new TH1F( "3---------------------------------------------", "", 1, 0.0, 1.0 );
  
  hTotalChargeHG_GeV = new TH1F( "hTotalChargeHG_GeV", "Total charge distribution", 500, 0.0, 500.0 );
  hTotalChargeLG_GeV = new TH1F( "hTotalChargeLG_GeV", "Total charge distribution", 500, 0.0, 500.0 );
  hTotalChargeCal_GeV = new TH1F( "hTotalChargeCal_GeV", "Total charge distribution", 500, 0.0, 500.0 );

  hSeparator[3] = new TH1F( "4---------------------------------------------", "", 1, 0.0, 1.0 );

  hChSaturation = new TH1F( "hChSaturation", "Channel Saturation", 1700, 0.0, 1700.0 );
  hChSaturationHG = new TH1F( "hChSaturationHG", "Channel saturation in HG only", 1700, 0.0, 1700.0 );
  hChSaturationLG = new TH1F( "hChSaturationLG", "Channel saturation in LG only", 1700, 0.0, 1700.0 );
  hChSaturationEvent = new TH1F( "hChSaturationEvent", "Channel saturation per event", 500, 0.0, 500.0 );
  hChSaturationEventHG = new TH1F( "hChSaturationEventHG", "Channel saturation per event in HG", 250, 0.0, 250.0 );
  hChSaturationEventLG = new TH1F( "hChSaturationEventLG", "Channel saturation per event in LG", 250, 0.0, 250.0 );

  hSeparator[4] = new TH1F( "5---------------------------------------------", "", 1, 0.0, 1.0 );

  hCalOGSlopes = new TH1F( "hCalOGSlopes", "Distribution of the slopes for the uncut graphs", 150, 0.0, 30.0 );
  hCalOGConsts = new TH1F( "hCalOGConsts", "Distribution of the constants for the uncut graphs", 200, -1000.0, 1000.0 );
  hCalMidSlopes = new TH1F( "hCalMidSlopes", "Distribution of the slopes for the middle calibration step", 150, 0.0, 30.0 );
  hCalMidConsts = new TH1F( "hCalMidConsts", "Distribution of the constants for the middle calibration step", 200, -1000.0, 1000.0 );
  hCalSlopes = new TH1F( "hCalSlopes", "Distribution of the slopes for the final calibration", 150, 0.0, 30.0 );
  hCalConsts = new TH1F( "hCalConsts", "Distribution of the constants for the final calibration", 200, -1000.0, 1000.0 );
  hCalOGPointCount = new TH1F( "hCalOGPointCount", "Distribution of the point counts in OG calibration graphs", 2000, 0.0, 200000 );
  hCalMidPointCount = new TH1F( "hCalMidPointCount", "Distribution of the point counts in middle step calibration graphs", 2000, 0.0, 200000 );
  hCalPointCount = new TH1F( "hCalPointCount", "Distribution of the point counts in final calibration graphs", 2000, 0.0, 200000 );

  hSeparator[5] = new TH1F( "6---------------------------------------------", "", 1, 0.0, 1.0 );

  hCenterModuleOccupancy = new TH2F( "hCenterModuleOccupancy", "Hit occupancy in center module", 7, 0, 7, 7, 0, 7 );

  for( int i = 1; i < 10; i++ )
  {
    if( i != 5 )
    {
      sprintf( hName, "hOutModuleOccupancy-%d", i );
      sprintf( hTitle, "Hit Occupancy in module %d", i );
      hOutModuleOccupancy[i] = new TH2F( hName, hTitle, 5, 0, 5, 5, 0, 5 );
    }
  }

  hSeparator[6] = new TH1F( "7---------------------------------------------", "", 1, 0.0, 1.0 );

  init();
}


Analyser::~Analyser()
{ 
  fOut->Write();
  fOut->Close();
}


double Analyser::calcChipTime( SRSData *srs, int ih )
{
  double BCIDRes = 25.;
  double TACSlope = 60.;
  double TDORange = 255.;
  
  return srs->hits[ih]->bcid * BCIDRes + ( 1.5 * BCIDRes - srs->hits[ih]->tdc * ( TACSlope / TDORange ) );
}


double Analyser::calcFECTime( SRSData *srs, int ih )
{
  double FEC_Time;
  
  for( int id = 1; id < ih; id++)
    {
      if( srs->hits[ih-id]->hasDataMarker == true ) { continue; }	
      
      if( 1.*srs->hits[ih]->vmmid == 1.*srs->hits[ih-id]->vmmid )
      {
	FEC_Time = 1.*srs->hits[ih-id]->fecTimeStamp * 22.5 + 1*srs->hits[ih]->triggerOffset * 4096 * 22.5;
	break;
      }
    }

  return FEC_Time;
}


double Analyser::calcTimeStamp( SRSData *srs, int ih )
{
  double Chip_Time = calcChipTime( srs, ih );
  double FEC_Time = calcFECTime( srs, ih );

  return FEC_Time + Chip_Time; 
}


int Analyser::gainSelector( int ch )
{
  if( CH_Mapper[0][ch] == 1 ) { return 0; }

  if( CH_Mapper[1][ch] == 1 ) { return 1; }

  return 2;
}


int Analyser::eventContinuity( SingleHit CurrHit, SRSData *srs, int ih, int nhits )
{
  int n = 10; // Number of forward steps to check

  int Gain = gainSelector( srs->hits[ih]->chno );
    
  SingleHit PrevHit;
  if( CurrentEvent[Gain].empty() == false )
  {
    for( int i = CurrentEvent[Gain].size() - 1; i >= 0; i-- )
    {
      if( PrevHit.vmm == CurrHit.vmm )
      {
	PrevHit = CurrentEvent[Gain][i];
	break;
      }
      
      else
	PrevHit = CurrentEvent[Gain].front();
    }
  }
  
  if( abs( CurrHit.ts_ns - PrevHit.ts_ns ) <= 1000. ) { return 0; } // If next hit is okay - it's okay
  else
  {
    
    int i = 1;
    
    while( i < n )
    {
      if( ih + n >= nhits ) { return -2; } // Make sure not to access non-existing array members
      
      if( srs->hits[ih+i]->hasDataMarker == false ) { n++; continue; }
      else
      {
	double TempTS = calcTimeStamp( srs, ih+i );
	
	if( abs( PrevHit.ts_ns - TempTS ) <= 8000. ) { return i; }
      }
      
      i++;
    }
  }

  return -1;
}


int Analyser::eventContinuity2( SingleHit CurrHit, SRSData *srs, int ih, int nhits )
{ 
  int n = 10; // Number of forward steps to check
  int Gain = gainSelector( srs->hits[ih]->chno );

  SingleHit FHit;
      FHit = CurrentEvent[Gain].front();
  
  if( abs( CurrHit.ts_ns - FHit.ts_ns ) <= 3000. ) { return 0; } // If next hit is okay - it's okay
  else
  {
    int i = 1;
    
    while( i < n )
    {
      if( ih + n >= nhits ) { return -2; } // Make sure not to access non-existing array members
      
      if( srs->hits[ih+i]->hasDataMarker == false ) { n++; continue; }
      else
      {
	double TempTS = calcTimeStamp( srs, ih+i );
	
	if( abs( FHit.ts_ns - TempTS ) <= 8000. ) { return i; }
      }
      
      i++;
    }
  }

  return -1;
}


void Analyser::drawHitMap()
{
  gPad->SetFrameLineColor( 0 );
  gPad->SetFrameLineWidth( 0 );
  gPad->SetTopMargin( 0.0 );
  gPad->SetBottomMargin( 0.0 );
  gPad->SetLeftMargin( 0.0 );
  gPad->SetRightMargin( 0.0 );
  gPad->SetTickx( 0 );
  gPad->SetTicky( 0 );
  
  TCanvas *hitOccupancyCanvas = new TCanvas( "hitOccupancy", "Hit Occupancy", 800, 800 );
  hitOccupancyCanvas->Divide( 3, 3 );

  hCenterModuleOccupancy->GetXaxis()->SetLabelSize( 0.0 );
  hCenterModuleOccupancy->GetYaxis()->SetLabelSize( 0.0 );
  hCenterModuleOccupancy->GetXaxis()->SetTitleSize( 0.0 );
  hCenterModuleOccupancy->GetYaxis()->SetTitleSize( 0.0 );
  gPad->SetLeftMargin( 0.0 );
  gPad->SetBottomMargin( 0.0 );

  hitOccupancyCanvas->cd( 5 );
  hCenterModuleOccupancy->Draw( "colz" );
  
  for( int i = 1; i < 10; i++ )
  {
    if( i != 5 )
    {
      if ( i % 3 != 1 )
	gPad->SetLeftMargin( 0.0 );
	
      if ( i <= 6 )
	gPad->SetBottomMargin( 0.0 );
      
      hOutModuleOccupancy[i]->GetXaxis()->SetLabelSize( 0.0 );
      hOutModuleOccupancy[i]->GetYaxis()->SetLabelSize( 0.0 );
      hOutModuleOccupancy[i]->GetXaxis()->SetTitleSize( 0.0 );
      hOutModuleOccupancy[i]->GetYaxis()->SetTitleSize( 0.0 );
      
      hitOccupancyCanvas->cd( i );
      hOutModuleOccupancy[i]->Draw( "colz" );
    }
  }

  fOut->cd();

  hitOccupancyCanvas->Modified();
  hitOccupancyCanvas->Update();

  hitOccupancyCanvas->Write( "DetectorHitMap" );
}


int Analyser::fForward( SRSData *srs, int ih )
{
  
  if( calcTimeStamp( srs, ih ) <= 1e7 )
    return 1;

  if( srs->hits[ih]->hasDataMarker == false )
    return 2;

  int gain = gainSelector( 1.*srs->hits[ih]->chno );
  if( gain == 2 )
  {
    std::cout << "VMM: " << 1.*srs->hits[ih]->vmmid << std::endl;
    std::cout << "Channel: " << 1.*srs->hits[ih]->chno << std::endl;
    std::cout << "Gain: " << gain << std::endl << std::endl;
    
    return 3;
  }

  return 0;
}


void Analyser::clearTestFile()
{
  std::ofstream ClFile( "Datastream.csv", std::ios::trunc ); 
  ClFile.close();
}


void Analyser::clearTreeFile()
{
  TFile *fTree = new TFile( "treeTemp.root", "RECREATE" );
  fTree->Close();
}
