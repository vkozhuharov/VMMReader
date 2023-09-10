#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <map>
#include <array>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <typeinfo>

#include "TCanvas.h"
#include <TString.h>

#include "Analyser.hh"



int cl_count = 0;
int pr_count = 0;
int prints = 0;
int fhitc = 0;
int outc = 0;
int begin_ih;
int end_ih;
float saturation = 1023;


std::vector< int > VMMs { 0, 1, 2, 3, 4, 5, 8, 9, 10,11, 12, 13 };
// VMM0, VMM1, VMM2, VMM3, VMM9, VMM8, VMM10, VMM11


struct Mapping
{
  int vmm;
  int hg_ch;
  int lg_ch;
  int canvas;
  int x;
  int y;
};



int Analyser::elem_index( auto array, int vmm )
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


void ReadMap( const std::string & filename )
{
  int vmm, canvas, x, y, hg, lg;
  std::string channels;

  std::string delimiter = "/";
  std::ifstream file( filename.c_str() );

  // std::unordered_map< std::array< int, 3 >, std::pair< int, int > > mapper;
  while( file >> vmm >> channels >> canvas >> x >> y )
  {
    size_t st = 0;
    size_t stop = channels.find( delimiter );

    hg = stoi( channels.substr( st, stop-st ) );
    lg = stoi( channels.substr( stop+1 ) );

    // std::cout << "HG: " << hg << "   LG: " << lg << std::endl;
    // mapper[{ vmm, hg, lg }] = { x, y };
  }
}



void Analyser::init()
{
  for( int i = 0; i < 16; i++ )
    for( int j = 0; j < 128; j++ ) { chmap[i][j] = -1; }

  for( int VMMID = 0; VMMID < VMMs.size(); VMMID += 2 )
  { // High Gain Channels                  // Low Gain Channels
    chmap[VMMs[VMMID]][63] = 0;            chmap[VMMs[VMMID]][61] = 64;
    chmap[VMMs[VMMID]][62] = 1;            chmap[VMMs[VMMID]][60] = 65;
    chmap[VMMs[VMMID]][59] = 2;            chmap[VMMs[VMMID]][57] = 66;
    chmap[VMMs[VMMID]][58] = 3;            chmap[VMMs[VMMID]][56] = 67;
    chmap[VMMs[VMMID]][55] = 4;            chmap[VMMs[VMMID]][53] = 68;
    chmap[VMMs[VMMID]][54] = 5;            chmap[VMMs[VMMID]][52] = 69;
    chmap[VMMs[VMMID]][51] = 6;            chmap[VMMs[VMMID]][49] = 70;
    chmap[VMMs[VMMID]][50] = 7;            chmap[VMMs[VMMID]][48] = 71;
    chmap[VMMs[VMMID]][47] = 8;            chmap[VMMs[VMMID]][45] = 72;
    chmap[VMMs[VMMID]][46] = 9;            chmap[VMMs[VMMID]][44] = 73;
    chmap[VMMs[VMMID]][43] = 10;           chmap[VMMs[VMMID]][41] = 74;
    chmap[VMMs[VMMID]][42] = 11;           chmap[VMMs[VMMID]][40] = 75;
    chmap[VMMs[VMMID]][39] = 12;           chmap[VMMs[VMMID]][37] = 76;
    chmap[VMMs[VMMID]][38] = 13;           chmap[VMMs[VMMID]][36] = 77;
    chmap[VMMs[VMMID]][35] = 14;           chmap[VMMs[VMMID]][33] = 78;
    chmap[VMMs[VMMID]][34] = 15;           chmap[VMMs[VMMID]][32] = 79;
    chmap[VMMs[VMMID]][31] = 16;           chmap[VMMs[VMMID]][29] = 80;
    chmap[VMMs[VMMID]][30] = 17;           chmap[VMMs[VMMID]][28] = 81;
    chmap[VMMs[VMMID]][27] = 18;           chmap[VMMs[VMMID]][25] = 82;
    chmap[VMMs[VMMID]][26] = 19;           chmap[VMMs[VMMID]][24] = 83;
    chmap[VMMs[VMMID]][23] = 20;           chmap[VMMs[VMMID]][21] = 84;
    chmap[VMMs[VMMID]][22] = 21;           chmap[VMMs[VMMID]][20] = 85;
    chmap[VMMs[VMMID]][19] = 22;           chmap[VMMs[VMMID]][17] = 86;
    chmap[VMMs[VMMID]][18] = 23;           chmap[VMMs[VMMID]][16] = 87;
    chmap[VMMs[VMMID]][15] = 24;           chmap[VMMs[VMMID]][13] = 88;
    chmap[VMMs[VMMID]][14] = 25;           chmap[VMMs[VMMID]][12] = 89;
    chmap[VMMs[VMMID]][11] = 26;           chmap[VMMs[VMMID]][9] = 90;
    chmap[VMMs[VMMID]][10] = 27;           chmap[VMMs[VMMID]][8] = 91;
    chmap[VMMs[VMMID]][7] = 28;            chmap[VMMs[VMMID]][5] = 92;
    chmap[VMMs[VMMID]][6] = 29;            chmap[VMMs[VMMID]][4] = 93;
    chmap[VMMs[VMMID]][3] = 30;            chmap[VMMs[VMMID]][1] = 94;
    chmap[VMMs[VMMID]][2] = 31;            chmap[VMMs[VMMID]][0] = 95;
    
    chmap[VMMs[VMMID+1]][63] = 32;         chmap[VMMs[VMMID+1]][61] = 96;
    chmap[VMMs[VMMID+1]][62] = 33;         chmap[VMMs[VMMID+1]][60] = 97;
    chmap[VMMs[VMMID+1]][59] = 34;         chmap[VMMs[VMMID+1]][57] = 98;
    chmap[VMMs[VMMID+1]][58] = 35;         chmap[VMMs[VMMID+1]][56] = 99;
    chmap[VMMs[VMMID+1]][55] = 36;         chmap[VMMs[VMMID+1]][53] = 100;
    chmap[VMMs[VMMID+1]][54] = 37;         chmap[VMMs[VMMID+1]][52] = 101;
    chmap[VMMs[VMMID+1]][51] = 38;         chmap[VMMs[VMMID+1]][49] = 102;
    chmap[VMMs[VMMID+1]][50] = 39;         chmap[VMMs[VMMID+1]][48] = 103;
    chmap[VMMs[VMMID+1]][47] = 40;         chmap[VMMs[VMMID+1]][45] = 104;
    chmap[VMMs[VMMID+1]][46] = 41;         chmap[VMMs[VMMID+1]][44] = 105;
    chmap[VMMs[VMMID+1]][43] = 42;         chmap[VMMs[VMMID+1]][41] = 106;
    chmap[VMMs[VMMID+1]][42] = 43;         chmap[VMMs[VMMID+1]][40] = 107;
    chmap[VMMs[VMMID+1]][39] = 44;         chmap[VMMs[VMMID+1]][37] = 108;
    chmap[VMMs[VMMID+1]][38] = 45;         chmap[VMMs[VMMID+1]][36] = 109;
    chmap[VMMs[VMMID+1]][35] = 46;         chmap[VMMs[VMMID+1]][33] = 110;
    chmap[VMMs[VMMID+1]][34] = 47;         chmap[VMMs[VMMID+1]][32] = 111;
    chmap[VMMs[VMMID+1]][31] = 48;         chmap[VMMs[VMMID+1]][29] = 112;
    chmap[VMMs[VMMID+1]][30] = 49;         chmap[VMMs[VMMID+1]][28] = 113;
    chmap[VMMs[VMMID+1]][27] = 50;         chmap[VMMs[VMMID+1]][25] = 114;
    chmap[VMMs[VMMID+1]][26] = 51;         chmap[VMMs[VMMID+1]][24] = 115;
    chmap[VMMs[VMMID+1]][23] = 52;         chmap[VMMs[VMMID+1]][21] = 116;
    chmap[VMMs[VMMID+1]][22] = 53;         chmap[VMMs[VMMID+1]][20] = 117;
    chmap[VMMs[VMMID+1]][19] = 54;         chmap[VMMs[VMMID+1]][17] = 118;
    chmap[VMMs[VMMID+1]][18] = 55;         chmap[VMMs[VMMID+1]][16] = 119;
    chmap[VMMs[VMMID+1]][15] = 56;         chmap[VMMs[VMMID+1]][13] = 120;
    chmap[VMMs[VMMID+1]][14] = 57;         chmap[VMMs[VMMID+1]][12] = 121;
    chmap[VMMs[VMMID+1]][11] = 58;         chmap[VMMs[VMMID+1]][9] = 122;
    chmap[VMMs[VMMID+1]][10] = 59;         chmap[VMMs[VMMID+1]][8] = 123;
    chmap[VMMs[VMMID+1]][7] = 60;          chmap[VMMs[VMMID+1]][5] = 124;
    chmap[VMMs[VMMID+1]][6] = 61;          chmap[VMMs[VMMID+1]][4] = 125;
    chmap[VMMs[VMMID+1]][3] = 62;          chmap[VMMs[VMMID+1]][1] = 126;
    chmap[VMMs[VMMID+1]][2] = 63;          chmap[VMMs[VMMID+1]][0] = 127;
  }
}



Analyser::Analyser()
{
  char hName[64];
  char hTitle[64];
  
  fout = new TFile( "output.root", "RECREATE" );

  hChIndex = new TH1F( "hCh", "Channel index", 128, 0.0, 128.0 );
  hChNumber = new TH1F( "hChNumber", "Channel number", 128, 0.0, 128.0 );
  hNHits = new TH1F( "hNHits", "Number of hits", 100, 0.0, 100.0 );
  
  hHitOccupancy = new TH2F( "hHitOccupancy", "Hit occupancy map", 17, 0, 17, 17, 0, 17 );
  hHitChargeOccupancy = new TH2F( "hHitChargeOccupancy", "Hit charge map", 17, 0, 17, 17, 0, 17 );
  hFilteredHitChargeOccupancy = new TH2F( "hFilteredOccupancy", "Filtered hit charge map", 17, 0, 17, 17, 0, 17 );
  hFilteredHitChargeOccupancy1 = new TH2F( "hFilteredOccupancy1", "Filtered hit charge map", 17, 0, 17, 17, 0, 17 );
  hSaturationMap = new TH2F( "hSaturationMap", "Saturation map", 17, 0, 17, 17, 0, 17 );

  hCharge = new TH1F( "hCharge", "Hit charge distribution", 120, 0.0, 1200.0 );
  hTotalCharge = new TH1F( "hTotalCharge", "Total charge distribution", 2500, 0.0, 60000.0 );
  hTotalChargeNS = new TH1F( "hTotalChargeNS", "Total charge distribution, filtered saturation", 2500, 0.0, 60000.0 );
  
  for ( int i = 0; i < 5; i++ )
  {
    sprintf( hName, "hTotalChargeFilt-%d", i+2 );
    sprintf( hTitle, "Total charge distribution with %d+ triggered channels", i+2 );
    hTotalChargeFilt[i] = new TH1F( hName, hTitle, 2500, 0.0, 60000.0 );
  }
  
  for ( int i = 0; i < 5; i++ )
  {
    sprintf( hName, "hHitSplitCharge %d", i+2 );
    sprintf( hTitle, "Total charge for 100000 events - %d", i+2 );
    hHitSplitCharge[i] = new TH1F( hName, hTitle, 2500, 0.0, 60000.0 );
  }

  int isat = 0;
  for ( auto id: VMMs )
  {
    sprintf( hName, "hSaturation-%2d", id );
    sprintf( hTitle, "Saturation distribution for VMM %d", id );
    hSaturation[isat] = new TH1F( hName, hTitle, 64, 0.0, 64.0 );
    isat++;
  }

  hTotalSaturation = new TH1F( "hTotalSaturation", "Saturated hits channel distribution", 80, 0.0, 80.0 );
  hEventSaturationPNZ = new TH1F( "hEventSaturationPNZ", "Percentage of saturation in event", 100, 0.0, 1.0 );
  hEventSaturationNZ = new TH1F( "hEventSaturationNZ", "Saturated channels in event", 1023, 0.0, 1023 );
  hEventSaturationP = new TH1F( "hEventSaturationP", "Percentage of saturation in event", 100, 0.0, 1.0 );
  hEventSaturation = new TH1F( "hEventSaturation", "Saturated channels in event", 1023, 0.0, 1023 );
  hSingleSatF = new TH1F( "hSingleSatF", "Saturated single-channel and two-channel events", 1200, 0.0, 1200 );

  hTotalCharge_Sat = new TH1F( "hTotalCharge_Sat", "Total charge distribution", 2500, 0.0, 60000.0 );
  
  init();
}



Analyser::~Analyser()
{
  // write the histograms and the file and exit
  fout->Write();
  fout->Close();;
}



void Analyser::ClearTestFile()
{
  std::ofstream ClFile( "Datastream.csv", std::ios::trunc ); 
  ClFile.close();
}



void Analyser::process( SRSData *srs )
{
  ReadMap( "VMM_Map.txt" );
  // const auto table = load_table ( "VMM_Map.txt" );
  static int iev;

  int nhits = srs->getHits().size();
  if( nhits < 500 ) { return; }
  
  
  for( int ih = 0; ih < nhits; ih++ )
  {
    bool fnd = false;
    for( auto chipid: VMMs )
    {
      if( srs->hits[ih]->vmmid == chipid )
      {
	hChIndex->Fill( 64.*srs->hits[ih]->vmmid + 1.*srs->hits[ih]->chno );
	fnd = true;
      }
    }

    if( fnd == false )
    {
      std::cout << "Unknown VMM chip: " << 1.*srs->hits[ih]->vmmid << std::endl;
      continue;
    }
    
    
    if( srs->hits[ih]->chno == -1 )
    {
      // std::cout << "Unknown channel: " << 1.*srs->hits[ih]->chno  << std::endl;
      continue;
    }

    
    int ch = chmap[srs->hits[ih]->vmmid][srs->hits[ih]->chno];      
    if( ch == -1 )
    {
      // may be for some noise estimations
      // std::cout << "Hit from unconnected channel: " << ch << std::endl;
      continue;
    }
      
    hChNumber->Fill( ch );
    // hCharge1[ch]->Fill( 1.*srs->hits[ih]->adc );

    
    // float time = 1.*srs->hits[ih]->bcid*25 + ( 1.5*25 - 1.*srs->hits[ih]->tdc*60/255 );

    
    if( 1.*srs->hits[ih]->adc <= 0 ) { continue; }

    
    int board;
    if( srs->hits[ih]->vmmid == 0 || srs->hits[ih]->vmmid == 1 ) { board = 2; }
    if( srs->hits[ih]->vmmid == 2 || srs->hits[ih]->vmmid == 3 ) { board = 3; }
    if( srs->hits[ih]->vmmid == 8 || srs->hits[ih]->vmmid == 9 ) { board = 0; }
    if( srs->hits[ih]->vmmid == 10 || srs->hits[ih]->vmmid == 11 ) { board = 1; }

    /*
    auto [x, y] = table.at( std::make_pair( board, ch ) );
    hHitOccupancy->Fill( x, y, 1. );
    hHitChargeOccupancy->Fill( x, y, 1.*srs->hits[ih]->adc );
    hCharge->Fill( 1.*srs->hits[ih]->adc );
    */
    
    int ev_bcid_hop;
    if( ih == 0 )
    {
      Hop_Buffer.vmm = 1.*srs->hits[ih]->vmmid;
      Hop_Buffer.channel = 1.*srs->hits[ih]->chno;
      Hop_Buffer.bcid = 1.*srs->hits[ih]->bcid;

      if( 1.*srs->hits[ih]->adc >= saturation )
	Hop_Buffer.charge = saturation;
	  
      else
	Hop_Buffer.charge = 1.*srs->hits[ih]->adc;

      ev_bcid_hop = 1.*srs->hits[ih]->bcid;
      begin_ih = ih;
    }

    else if( ih > 0 && ih < nhits-6 )
    {
      if( abs( 1.*srs->hits[ih]->bcid - ev_bcid_hop ) <= 20 )
      {
	Hop_Buffer.vmm = 1.*srs->hits[ih]->vmmid;
	Hop_Buffer.channel = 1.*srs->hits[ih]->chno;
	Hop_Buffer.bcid = 1.*srs->hits[ih]->bcid;

	if( 1.*srs->hits[ih]->adc >= saturation )
	  Hop_Buffer.charge = saturation;
	  
	else
	  Hop_Buffer.charge = 1.*srs->hits[ih]->adc;
	
	Hop_Event.push_back( Hop_Buffer );
      }

      else if( ih > 0 && ih < nhits-6 )
      {
	bool found = false;
	for( int fih = 1; fih < 6; fih++ )
	{ 
	  if( abs( 1.*srs->hits[ih+fih]->bcid - ev_bcid_hop ) <= 20 )
	  {
	    Hop_Buffer.vmm = 1.*srs->hits[ih]->vmmid;
	    Hop_Buffer.channel = 1.*srs->hits[ih]->chno;
	    Hop_Buffer.bcid = 1.*srs->hits[ih]->bcid;
	    
	    if( 1.*srs->hits[ih]->adc >= saturation )
	      Hop_Buffer.charge = saturation;
	    
	    else
	      Hop_Buffer.charge = 1.*srs->hits[ih]->adc;
	    
	    Hop_Outliers.push_back( Hop_Buffer );

	    found = true;
	  }
	}

	if( found == false )
	{
	  int TotCharge_Hop = 0;
	  int saturated = 0;
	  
	  end_ih = ih;


	  // Total Charge
	  for( auto &EvHit: Hop_Event )
	  {
	    TotCharge_Hop += EvHit.charge;
	    
	    if( Hop_Event.size() > 20 && EvHit.charge >= saturation-1 )
	    {
	      saturated++;
	      hSingleSatF->Fill( 100*EvHit.vmm + EvHit.channel );
	    }
	  }

	  
	  if( saturated > 0 )
	  {	
	    hEventSaturationPNZ->Fill( saturated / ( 1.*Hop_Event.size() ) );
	    hEventSaturationNZ->Fill( saturated );
	  }
	  
	  hEventSaturationP->Fill( saturated / ( 1.*Hop_Event.size() ) );
	  hEventSaturation->Fill( saturated );

	  if( Hop_Event.size() > 20 )
	    hTotalCharge_Sat->Fill( TotCharge_Hop );

	  
	  // Fill in the Total Charge Histograms
	  if( TotCharge_Hop > 150 ) { hTotalCharge->Fill( TotCharge_Hop ); }
	  
	  for( int i = 0; i < 5; i++ )
	  {
	    if( TotCharge_Hop > 150 && Hop_Event.size() >= i+2 )
	      hTotalChargeFilt[i]->Fill( TotCharge_Hop );
	  }


	  // Clear Event vector and start "collecting" the next event
	  Hop_Event.clear();
	
	  Hop_Buffer.vmm = 1.*srs->hits[ih]->vmmid;
	  Hop_Buffer.channel = 1.*srs->hits[ih]->chno;
	  Hop_Buffer.bcid = 1.*srs->hits[ih]->bcid;
	  Hop_Buffer.charge = 1.*srs->hits[ih]->adc;
	  
	  Hop_Event.push_back( Hop_Buffer );
	  
	  ev_bcid_hop = 1.*srs->hits[ih]->bcid;
	  begin_ih = ih;
	}
      }
      
      
      int vmm;
      int sat_index;
      if( 1.*srs->hits[ih]->adc >= saturation-1 )
      {
	vmm = 1.*srs->hits[ih]->vmmid;
	sat_index = elem_index( VMMs, vmm );
	
	if ( sat_index != -1 ) { hSaturation[sat_index]->Fill( ch ); }
	hTotalSaturation->Fill( ch );
	//hSaturationMap->Fill( x, y, 1. );
      }
    }
  }
  
  // fhitc += nhits;   // Full hit count
  // outc += Hop_Outliers.size();   // Outliers count

  iev++;
}



void Analyser::plotting()
{
  char fname[64];

  TCanvas ChargeCanvas = new TCanvas();
  TCanvas MapCanvas = new TCanvas();
  
  ChargeCanvas.cd();
  hTotalCharge->Draw();
  sprintf( fname, "TotalCharge.png" );
  ChargeCanvas.Print( fname );
  
  MapCanvas.cd();
  hHitChargeOccupancy->Draw( "colz" );
  sprintf( fname, "ChargeOccupancyMap.png" );
  MapCanvas.Print( fname );
}
