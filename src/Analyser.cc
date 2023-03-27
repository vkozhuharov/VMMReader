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
float saturation = 500;


std::vector< int > VMMs { 1, 0, 3, 2, 9, 8, 11, 10 };
// VMM0, VMM1, VMM2, VMM3, VMM9, VMM8, VMM10, VMM11


std::map< std::pair< int, int >, std::pair< int, int > >
load_table( const std::string &filename )
{
  int b, c, x, y;
  std::map< std::pair< int, int >, std::pair< int, int > > table;
  std::ifstream fdata( filename.c_str() );
  while( fdata >> b >> c >> x >> y )
  {
    table[{ b, c }] = { x, y }; // Read the table and store the values 
  }
    
  return table;
}

const auto table = load_table ( "mapping_total.txt" );



int elem_index( auto array, int vmm )
{
  for( int i = 0; i < array.size(); i++ )
  {
    // std::cout << VMMs[i] << std::endl;
    if( array[i] == vmm ) { return i; }
  }

  return -1;
}



void Analyser::init()
{
  for( int i = 0; i < 16; i++ )
  {
    for( int j = 0; j < 64; j++ ) { chmap[i][j] = -1; }
  }

  for( int VMMID = 0; VMMID < VMMs.size(); VMMID += 2 )
  {
    chmap[VMMs[VMMID]][63] = 0;
    chmap[VMMs[VMMID]][62] = 1;
    chmap[VMMs[VMMID]][59] = 2;
    chmap[VMMs[VMMID]][58] = 3;
    chmap[VMMs[VMMID]][55] = 4;
    chmap[VMMs[VMMID]][54] = 5;
    chmap[VMMs[VMMID]][51] = 6;
    chmap[VMMs[VMMID]][50] = 7;
    chmap[VMMs[VMMID]][47] = 8;
    chmap[VMMs[VMMID]][46] = 9;
    chmap[VMMs[VMMID]][43] = 10;
    chmap[VMMs[VMMID]][42] = 11;
    chmap[VMMs[VMMID]][39] = 12;
    chmap[VMMs[VMMID]][38] = 13;
    chmap[VMMs[VMMID]][35] = 14;
    chmap[VMMs[VMMID]][34] = 15;
    chmap[VMMs[VMMID]][31] = 16;
    chmap[VMMs[VMMID]][30] = 17;
    chmap[VMMs[VMMID]][27] = 18;
    chmap[VMMs[VMMID]][26] = 19;
    chmap[VMMs[VMMID]][23] = 20;
    chmap[VMMs[VMMID]][22] = 21;
    chmap[VMMs[VMMID]][19] = 22;
    chmap[VMMs[VMMID]][18] = 23;
    chmap[VMMs[VMMID]][15] = 24;
    chmap[VMMs[VMMID]][14] = 25;
    chmap[VMMs[VMMID]][11] = 26;
    chmap[VMMs[VMMID]][10] = 27;
    chmap[VMMs[VMMID]][7] = 28;
    chmap[VMMs[VMMID]][6] = 29;
    chmap[VMMs[VMMID]][3] = 30;
    chmap[VMMs[VMMID]][2] = 31; 
    
    chmap[VMMs[VMMID+1]][63] = 32;
    chmap[VMMs[VMMID+1]][62] = 33;
    chmap[VMMs[VMMID+1]][59] = 34;
    chmap[VMMs[VMMID+1]][58] = 35;
    chmap[VMMs[VMMID+1]][55] = 36;
    chmap[VMMs[VMMID+1]][54] = 37;
    chmap[VMMs[VMMID+1]][51] = 38;
    chmap[VMMs[VMMID+1]][50] = 39;
    chmap[VMMs[VMMID+1]][47] = 40;
    chmap[VMMs[VMMID+1]][46] = 41;
    chmap[VMMs[VMMID+1]][43] = 42;
    chmap[VMMs[VMMID+1]][42] = 43;
    chmap[VMMs[VMMID+1]][39] = 44;
    chmap[VMMs[VMMID+1]][38] = 45;
    chmap[VMMs[VMMID+1]][35] = 46;
    chmap[VMMs[VMMID+1]][34] = 47;
    chmap[VMMs[VMMID+1]][31] = 48;
    chmap[VMMs[VMMID+1]][30] = 49;
    chmap[VMMs[VMMID+1]][27] = 50;
    chmap[VMMs[VMMID+1]][26] = 51;
    chmap[VMMs[VMMID+1]][23] = 52;
    chmap[VMMs[VMMID+1]][22] = 53;
    chmap[VMMs[VMMID+1]][19] = 54;
    chmap[VMMs[VMMID+1]][18] = 55;
    chmap[VMMs[VMMID+1]][15] = 56;
    chmap[VMMs[VMMID+1]][14] = 57;
    chmap[VMMs[VMMID+1]][11] = 58;
    chmap[VMMs[VMMID+1]][10] = 59;
    chmap[VMMs[VMMID+1]][7] = 60;
    chmap[VMMs[VMMID+1]][6] = 61;
    chmap[VMMs[VMMID+1]][3] = 62;
    chmap[VMMs[VMMID+1]][2] = 63; 
  }
}



Analyser::Analyser()
{
  char hName[64];
  char hTitle[64];
  
  fout = new TFile( "output.root", "RECREATE" );

  hChIndex = new TH1F( "hCh", "Channel index", 128, 0.0, 128.0 );
  hChNumber = new TH1F( "hChNumber", "Channel number", 128, 0.0, 128.0 );
  // hEventChNumber = new TH1F( "hEventChNumber", "Channel number", 128, 0.0, 128.0 );
  hNHits = new TH1F( "hNHits", "Number of hits", 100, 0.0, 100.0 );
  // hNHitsEvent = new TH1F ( "hNHitsEvent", "Hits per event", 100, 0.0, 1200 );
  
  // hChOccupancy = new TH2F( "hChOccupancy", "Channel occupancy", 8, 0, 8, 8, 0, 8 );
  // hEventChOccupancy = new TH2F( "hEventChOccupancy", "Channel occupancy", 8, 0, 8, 8, 0, 8 );
  // hChChargeOccupancy = new TH2F( "hChChargeOccupancy", "Charge occupancy map", 8, 0, 8, 8, 0, 8 );
  // hEventChChargeOccupancy = new TH2F( "hEventChChargeOccupancy", "Cumulative event charge", 8, 0, 8, 8, 0, 8 );
  
  hHitOccupancy = new TH2F( "hHitOccupancy", "Hit occupancy map", 17, 0, 17, 17, 0, 17 );
  hHitChargeOccupancy = new TH2F( "hHitChargeOccupancy", "Hit charge map", 17, 0, 17, 17, 0, 17 );
  hFilteredHitChargeOccupancy = new TH2F( "hFilteredOccupancy", "Filtered hit charge map", 17, 0, 17, 17, 0, 17 );
  hFilteredHitChargeOccupancy1 = new TH2F( "hFilteredOccupancy1", "Filtered hit charge map", 17, 0, 17, 17, 0, 17 );
  hSaturationMap = new TH2F( "hSaturationMap", "Saturation map", 17, 0, 17, 17, 0, 17 );

  hCharge = new TH1F( "hCharge", "Hit charge distribution", 120, 0.0, 1200.0 );
  hTotalCharge = new TH1F( "hTotalCharge", "Total charge distribution", 2500, 0.0, 60000.0 );
  hTotalCharge_Skip = new TH1F( "hTotalCharge_Skip", "Total charge distribution", 2500, 0.0, 60000.0 );
  hTotalCharge_Hop = new TH1F( "hTotalCharge_Hop", "Total charge distribution", 2500, 0.0, 60000.0 );
  hTotalChargeNS = new TH1F( "hTotalChargeNS", "Total charge distribution, filtered saturation", 2500, 0.0, 60000.0 );
  
  for ( int i = 0; i < 5; i++ )
  {
    sprintf( hName, "hTotalChargeFilt-%d", i+2 );
    sprintf( hTitle, "Total charge distribution with %d+ triggered channels", i+2 );
    hTotalChargeFilt[i] = new TH1F( hName, hTitle, 2500, 0.0, 60000.0 );
  }

  for ( int i = 0; i < 5; i++ )
  {
    sprintf( hName, "hTotalChargeFilt_Skip-%d", i+2 );
    sprintf( hTitle, "Total charge distribution with %d+ triggered channels", i+2 );
    hTotalChargeFilt_Skip[i] = new TH1F( hName, hTitle, 2500, 0.0, 60000.0 );
  }

  for ( int i = 0; i < 5; i++ )
  {
    sprintf( hName, "hTotalChargeFilt_Hop-%d", i+2 );
    sprintf( hTitle, "Total charge distribution with %d+ triggered channels", i+2 );
    hTotalChargeFilt_Hop[i] = new TH1F( hName, hTitle, 2500, 0.0, 60000.0 );
  }
  
  for ( int i = 0; i < 5; i++ )
  {
    sprintf( hName, "hHitSplitCharge %d", i+2 );
    sprintf( hTitle, "Total charge for 100000 events - %d", i+2 );
    hHitSplitCharge[i] = new TH1F( hName, hTitle, 2500, 0.0, 60000.0 );
  }

  /*
  for ( int i = 0; i < 64; i++ )
  {
    sprintf( hName, "hCharge1-%2d", i );
    sprintf( hTitle, "Charge distribution in channel %d", i );
    hCharge1[i] = new TH1F( hName, hTitle, 100, 0.0, 1200 );
  }
  */

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

  /*
  int vmm_c = 0;
  for ( auto &id: VMMs )
  {
    sprintf( hName, "hSingleSat_%d", id );
    sprintf( hTitle, "Saturated single-channel events, VMM %d", id );
    hSingleSat[vmm_c] = new TH1F( hName, hTitle, 64, 0.0, 64.0 );
  }
  */
  
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
  // std::ofstream OutFile( "Datastream.csv", std::ios::app );

  
  static int iev;
  int bcid_buf;
  bool skipped;
  int evt_charge = 0;
  int evt_charge_skip = 0;
  int evt_charge_buff_skip = 0;
  int evt_charge_hop = 0;

  struct Hit_Container
  {
    int vmm;
    int channel;
    int bcid;
    int charge;
  } Hop_Buffer;

  std::vector< Hit_Container > Hop_Event;
  std::vector< Hit_Container > Hop_Outliers;

  int nhits = srs->getHits().size();
  // hNHits->Fill( nhits );

  // hEventChOccupancy->Reset();
  // hEventChChargeOccupancy->Reset();

  
  for( int ih = 0; ih < nhits; ih++ )
  {
    hChIndex->Fill( 64.*srs->hits[ih]->vmmid + 1.*srs->hits[ih]->chno );

    
    if( srs->hits[ih]->vmmid != 0 && srs->hits[ih]->vmmid != 1 && srs->hits[ih]->vmmid != 2 && srs->hits[ih]->vmmid != 3 && srs->hits[ih]->vmmid != 10 && srs->hits[ih]->vmmid != 11 && srs->hits[ih]->vmmid != 8 && srs->hits[ih]->vmmid != 9 )
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
    
    auto [x, y] = table.at( std::make_pair( board, ch ) );
    hHitOccupancy->Fill( x, y, 1. );
    hHitChargeOccupancy->Fill( x, y, 1.*srs->hits[ih]->adc );
    hCharge->Fill( 1.*srs->hits[ih]->adc );


    int trig_count;
    if( ih != 0 )
    {
      if( abs( 1.*srs->hits[ih]->bcid - 1.*srs->hits[ih-1]->bcid ) <= 150 )
      {
	if( 1.*srs->hits[ih]->adc >= saturation )
	  evt_charge += saturation;
	  
	else
	  evt_charge += 1.*srs->hits[ih]->adc;	

	trig_count++;
      }
      
      else
      {
	bool sat;
	( trig_count == 1 && evt_charge == saturation )
	  ? sat = true
	  : sat = false;
	
	if( evt_charge != 0 )
	  hTotalCharge->Fill( evt_charge );
	
	if( evt_charge != 0 && sat == false )
	  hTotalChargeNS->Fill( evt_charge );

	
	for( int i = 0; i < 5; i++ )
	{
	  if( evt_charge != 0 && trig_count >= i+2 )
	  {
	    hTotalChargeFilt[i]->Fill( evt_charge );

	    if( i == 1 )
	      hFilteredHitChargeOccupancy->Fill( x, y, 1.*srs->hits[ih]->adc );
	    
	    if( i == 2 )
	      hFilteredHitChargeOccupancy1->Fill( x, y, 1.*srs->hits[ih]->adc );

	    if( i == 4 )
	    {
	      if( cl_count <= 100000 ) { hHitSplitCharge[0]->Fill( evt_charge ); }
	      if( cl_count > 100000 && cl_count <= 200000 ) { hHitSplitCharge[1]->Fill( evt_charge ); }
	      if( cl_count > 200000 && cl_count <= 300000 ) { hHitSplitCharge[2]->Fill( evt_charge ); }
	      if( cl_count > 300000 && cl_count <= 400000 ) { hHitSplitCharge[3]->Fill( evt_charge ); }
	      if( cl_count > 400000 ) { hHitSplitCharge[4]->Fill( evt_charge ); }
	      cl_count++;
	    }
	  }
	}

	if( 1.*srs->hits[ih]->adc >= saturation )
	  evt_charge = saturation;
	  
	else
	  evt_charge = 1.*srs->hits[ih]->adc;

	trig_count = 1;
      }
    }
    
    else
    {
      if( 1.*srs->hits[ih]->adc >= saturation )
	evt_charge = saturation;
	  
      else
	evt_charge = 1.*srs->hits[ih]->adc;
      
      trig_count = 1;
    }

    
    int trig_count_skip;
    int trig_buff_skip;
    if( ih > 1 && ih < nhits - 1 )
    {
      if( skipped == true )
      {
	if( abs( 1.*srs->hits[ih]->bcid - 1.*srs->hits[ih-2]->bcid ) <= 10 )
	{
	  if( 1.*srs->hits[ih]->adc >= saturation )
	    evt_charge_skip += saturation;
	  
	  else
	    evt_charge_skip += 1.*srs->hits[ih]->adc;

	  trig_count_skip++;
	  skipped = false;
	}

	else
	{
	  if( evt_charge_skip != 0 )
	    hTotalCharge_Skip->Fill( evt_charge_skip );
	  
	  if( evt_charge_buff_skip != 0 )
	    hTotalCharge_Skip->Fill( evt_charge_buff_skip );
	
	  for( int i = 0; i < 5; i++ )
	  {
	    if( evt_charge_skip != 0 && trig_count_skip >= i+2 )
	      hTotalChargeFilt_Skip[i]->Fill( evt_charge_skip );

	    if( evt_charge_buff_skip != 0 && trig_buff_skip >= i+2 )
	      hTotalChargeFilt_Skip[i]->Fill( evt_charge_buff_skip );
	  }

	  if( 1.*srs->hits[ih]->adc >= saturation )
	    evt_charge_skip = saturation;
	  
	  else
	    evt_charge_skip = 1.*srs->hits[ih]->adc;
	  
	  trig_count_skip = 1;
	  evt_charge_buff_skip = 0;
	  trig_buff_skip = 0;
	}
      }
      
      else if( abs( 1.*srs->hits[ih]->bcid - 1.*srs->hits[ih-1]->bcid ) <= 10 )
      {
	if( 1.*srs->hits[ih]->adc >= saturation )
	  evt_charge_skip += saturation;
	
	else
	  evt_charge_skip += 1.*srs->hits[ih]->adc;	

	trig_count_skip++;
      }

      else if( abs( 1.*srs->hits[ih+1]->bcid - 1.*srs->hits[ih-1]->bcid ) <= 10 )
      {
	if( 1.*srs->hits[ih]->adc >= saturation )
	  evt_charge_buff_skip += saturation;
	  
	else
	  evt_charge_buff_skip += 1.*srs->hits[ih]->adc;	

	trig_buff_skip++;
	skipped = true;
      }
	
      else
      {
	if( evt_charge_skip != 0 ) { hTotalCharge_Skip->Fill( evt_charge_skip ); }
	if( evt_charge_buff_skip != 0 ) { hTotalCharge_Skip->Fill( evt_charge_buff_skip ); }
	
	for( int i = 0; i < 5; i++ )
	{
	  if( evt_charge_skip != 0 && trig_count_skip >= i+2 )
	    hTotalChargeFilt_Skip[i]->Fill( evt_charge_skip );

	  if( evt_charge_buff_skip != 0 && trig_buff_skip >= i+2 )
	    hTotalChargeFilt_Skip[i]->Fill( evt_charge_buff_skip );
	}

	if( 1.*srs->hits[ih]->adc >= saturation )
	  evt_charge_skip = saturation;
	  
	else
	  evt_charge_skip = 1.*srs->hits[ih]->adc;
	
	trig_count_skip = 1;
	evt_charge_buff_skip = 0;
	trig_buff_skip = 0;
      }
    }

    else
    {
      if( 1.*srs->hits[ih]->adc >= saturation )
	evt_charge_skip = saturation;
	  
      else
	evt_charge_skip = 1.*srs->hits[ih]->adc;
      
      trig_count_skip = 1;
    }

    
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

    else if( ih > 0 && ih < nhits-1 )
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
	  
	  for( auto &EvHit: Hop_Event )
	    TotCharge_Hop += EvHit.charge;
	  
	  
	  for( auto &EvHit2: Hop_Event )
	  {	      
	    if( ( EvHit2.charge >= saturation-1 && Hop_Event.size() == 1 ) || ( TotCharge_Hop >= 2*saturation-1 && TotCharge_Hop <= 2*saturation+1 && Hop_Event.size() == 2 ) )
	    {
	      hSingleSatF->Fill( 100*EvHit2.vmm + EvHit2.channel );
	      hSingleSatF->Fill( 100*EvHit2.vmm + EvHit2.channel );
	    }
	  }
	  
	  /*
	    if( TotCharge_Hop <= 500 && begin_ih > 10 && end_ih < nhits - 10 && nhits > 50)
	    {
	    for( int evih = begin_ih - 5; evih <= end_ih + 5; evih++ )
	    {
	    if( evih == begin_ih || evih == end_ih ) { std::cout << "----------" << std::endl; }
	    
	    std::cout << "Hit: " << evih << ", ";
	    std::cout << "BCID: " << 1.*srs->hits[evih]->bcid << ", ";
	    std::cout << "Charge: " << 1.*srs->hits[evih]->adc;
	    std::cout << std::endl;
	    }
	    
	    std::cout << std::endl;
	    }
	  */
	  
	  
	  if( TotCharge_Hop > 150 ) { hTotalCharge_Hop->Fill( TotCharge_Hop ); }
	  
	  for( int i = 0; i < 5; i++ )
	  {
	    if( TotCharge_Hop > 150 && Hop_Event.size() >= i+2 )
		hTotalChargeFilt_Hop[i]->Fill( TotCharge_Hop );
	  }
	  	  
	  
	  if( Hop_Event.size() > 20 )
	  {
	    for( auto &EvHit: Hop_Event )
	    {
	      if( EvHit.charge >= saturation-1 )
		saturated += 1;
	    }
	    
	    if( saturated > 0 )
	    {	
	      hEventSaturationPNZ->Fill( saturated / ( 1.*Hop_Event.size() ) );
	      hEventSaturationNZ->Fill( saturated );
	    }
	    
	    hEventSaturationP->Fill( saturated / ( 1.*Hop_Event.size() ) );
	    hEventSaturation->Fill( saturated );
	    
	    hTotalCharge_Sat->Fill( TotCharge_Hop );
	  }
	  
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
	hSaturationMap->Fill( x, y, 1. );
      }
    }
  }

  // std::cout << "Initial Size: " << nhits << ", ";
// std::cout << "Outliers: " << Outliers.size();
  
  fhitc += nhits;
  outc += Hop_Outliers.size();
// std::cout << "     Current Total: " << fhitc << " / " << outc << std::endl;
  
  iev++;
}



void Analyser::plotting()
{
  char fname[64];
  
  TCanvas c1;
  c1.cd();
  
  // hEventChOccupancy->Draw( "colz" );
  sprintf( fname, "ocmap.png" );
  c1.Print( fname );
  
  // hEventChChargeOccupancy->Draw( "colz" );
  sprintf( fname, "cmap.png" );
  c1.Print( fname );
  
  hCharge->Draw( "colz" );
  sprintf( fname, "hist.png" );
  c1.Print( fname );
}
