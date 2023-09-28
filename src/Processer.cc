#include <algorithm>
#include <iostream>
#include <typeinfo>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include <array>
#include <map>

#include "TCanvas.h"
#include "TString.h"
#include "TTree.h"

#include "Analyser.hh"


void Analyser::clearEvent( auto &EventArray )
{
  for( int i = 0; i < 3; i++ )
    EventArray[i].clear();
}


void Analyser::process( SRSData *srs, int batchCount )
{
  const auto table = mapLoader( "VMM_Map.txt" );
  std::array< std::vector< SingleHit >, 3 > CurrentEvent;          // 0 -> HG, 1 -> LG, 2 -> errors
  
  int nhits = srs->getHits().size();
  if( nhits < 500 ) { return; }        // Ignore small packets ( noise )
  
  for( int ih = 0; ih < nhits; ih++ )
  {
    if( int test = fForward( srs, ih ) != 0 ) { continue; }   // Skips markers, bad events and so on

    SingleHit CurrHit;

    CurrHit.vmm = 1*srs->hits[ih]->vmmid;
    CurrHit.channel = 1*srs->hits[ih]->chno;
    CurrHit.charge = 1*srs->hits[ih]->adc;
    CurrHit.bcid = 1*srs->hits[ih]->bcid;
    CurrHit.tdo = 1*srs->hits[ih]->tdc;
    CurrHit.overflow = 1*srs->hits[ih]->triggerOffset;
    CurrHit.ts_ns = calcTimeStamp( srs, ih );

    hChActivation->Fill( 100.*CurrHit.vmm + CurrHit.channel );   // Fill in the channel activation
    
    int Gain = gainSelector( CurrHit.channel );
    if( Gain == 0 )
      CurrHit.channelIsLG = false;
    else if( Gain == 1 )
      CurrHit.channelIsLG = true;
    
    if ( CurrentEvent[Gain].empty() ) { CurrentEvent[Gain].push_back( CurrHit ); }
    else
    { 
      int continuity = eventContinuity( CurrHit, srs, ih, nhits );   // Properly define event
      
      if( continuity == 0 )
      {
	CurrentEvent[Gain].push_back( CurrHit );
      }
      
      else if( continuity > 0 )
      {
	ih += continuity-1;
	continue;
      }
    
      else
      {
	int TCharge_HG = 0;        // Stores totCharge for HG
	int TCharge_LG = 0;        // Stores totCharge for LG
	int eventRepCountHG = 0;   // Counts channel activation repetitions for HG in a single event
	int eventRepCountLG = 0;   // Counts channel activation repetitions for LG in a single event
	int eventSatCountHG = 0;   // Counts channel saturations for HG in a single event
	int eventSatCountLG = 0;   // Counts channel saturations for LG in a single event
	
	std::array< std::array< int, 64 >, 16 > Calibrator = {0};
	// Stores values for LG->HG linking and repetition recognition
	
	if( CurrentEvent[0].empty() == false )
	{
	  for( int i = 1; i < CurrentEvent[0].size() - 1; i++ )
	  {
	    if( Calibrator[CurrentEvent[0][i].vmm][CurrentEvent[0][i].channel] == 0 )
	    {
	      TCharge_HG += CurrentEvent[0][i].charge;
	      
	      Calibrator[CurrentEvent[0][i].vmm][CurrentEvent[0][i].channel]
		= CurrentEvent[0][i].charge;
	    }
	    
	    else
	    {
	      hChRepetition->Fill( 100*CurrentEvent[0][i].vmm + CurrentEvent[0][i].channel );
	      hChRepetitionHG->Fill( 100*CurrentEvent[0][i].vmm + CurrentEvent[0][i].channel );
	      // Fill in the HG channel repetition histograms

	      eventRepCountHG += 1;
	      continue;
	    }

	    if( CurrentEvent[0][i].charge >= saturation )
	    {
	      hChSaturation->Fill( 100*CurrentEvent[0][i].vmm + CurrentEvent[0][i].channel );
	      hChSaturationHG->Fill( 100*CurrentEvent[0][i].vmm + CurrentEvent[0][i].channel );
	      // Fill in the HG channel saturation histograms
	      
	      eventSatCountHG += 1;
	    }

	    auto it = table.find( std::make_pair( CurrentEvent[0][i].vmm, CurrentEvent[0][i].channel ) );

	    if( it != table.end() )
	    {
	      std::array< int, 3 > mapVals = it->second;

	      if( mapVals[0] == 5 )
		hCenterModuleOccupancy->Fill( mapVals[1], mapVals[2] );

	      else
		hOutModuleOccupancy[mapVals[0]]->Fill( mapVals[2], mapVals[1] );
	    }
	  }
	  
	  if( TCharge_HG > 0 )
	    hTotalChargeHG->Fill( TCharge_HG );   // Fill in the HG totCharge histogram

	  if( TCharge_HG > 0 )
	    hTotalChargeHG_GeV->Fill( ( TCharge_HG - enGeVHG[1] ) / enGeVHG[0] );
            // Fill in the GeV HG totCharge histogram

	  if( CurrentEvent[0].size() >= 10 )
	    hTotalChargeHG_F->Fill( TCharge_HG );   // Fill in a filtered HG totCharge histogram

	  if( eventRepCountHG > 0 )
	    hChRepetitionEventHG->Fill( eventRepCountHG );   // Fill in the per event HG repetition histogram

	  if( eventSatCountHG > 0 )
	    hChSaturationEventHG->Fill( eventSatCountHG );   // Fill in the per event HG saturation histogram
	}
	
	if( CurrentEvent[1].empty() == false )
	{ 
	  for( int j = 1; j < CurrentEvent[1].size() - 1; j++ )
	  {
	    if( Calibrator[CurrentEvent[1][j].vmm][CurrentEvent[1][j].channel] == 0 )
	    {
	      TCharge_LG += CurrentEvent[1][j].charge;

	      Calibrator[CurrentEvent[1][j].vmm][CurrentEvent[1][j].channel]
		= CurrentEvent[1][j].charge;
	    }

	    else
	    {
	      hChRepetition->Fill( 100*CurrentEvent[1][j].vmm + CurrentEvent[1][j].channel );
	      hChRepetitionLG->Fill( 100*CurrentEvent[1][j].vmm + CurrentEvent[1][j].channel );
	      // Fill in the LG channel repetition histograms

	      eventRepCountLG += 1;
	      continue;
	    }

	    if( CurrentEvent[1][j].charge >= saturation )
	    {
	      hChSaturation->Fill( 100*CurrentEvent[1][j].vmm + CurrentEvent[1][j].channel );
	      hChSaturationLG->Fill( 100*CurrentEvent[1][j].vmm + CurrentEvent[1][j].channel );
	      // Fill in the LG channel saturation histograms
	      
	      eventSatCountLG += 1;
	    }
	    
	    int hg_chan = CurrentEvent[1][j].channel + 2;
	    int hg_charge = Calibrator[CurrentEvent[1][j].vmm][hg_chan];
	      
	    if( hg_charge != 0 )
	    {
	      int vmm = CurrentEvent[1][j].vmm;
	      int channel = CurrentEvent[1][j].channel;
	      
	      if( ogMap.find( std::make_pair( vmm, channel ) ) == ogMap.end() )
	      {
		MapGraph2 *ogFiller = new MapGraph2;
		TGraph *newOGGraph = new TGraph;

		ogFiller->vmm = vmm;
		ogFiller->channel = channel;
		ogFiller->graph = newOGGraph;
		
		ogMap[std::make_pair( vmm, channel )].graph = newOGGraph;
		ogGraphs.push_back( ogFiller );

		// delete ogFiller;
		// delete newOGGraph;
	      }

	      auto og_it = ogMap.find( std::make_pair( vmm, channel ) );
	      TGraph* ogGraph = og_it->second.graph;

	      if( hg_charge < 1023 )
	      {
		int og_np = ogGraph->GetN();
		ogGraph->SetPoint( og_np, CurrentEvent[1][j].charge, hg_charge );
	      }
	    }
	  }

	  if( TCharge_LG > 0 )
	    hTotalChargeLG->Fill( TCharge_LG );   // Fill in the LG totCharge histogram

	  if( CurrentEvent[1].size() >= 8 )
	    hTotalChargeLG_F->Fill( TCharge_LG );   // Fill in a filtered LG totCharge histogram

	  if( eventRepCountLG > 0 )
	    hChRepetitionEventLG->Fill( eventRepCountLG );   // Fill in the per event LG repetition histogram

	  if( eventSatCountLG > 0 )
	    hChSaturationEventLG->Fill( eventSatCountLG );   // Fill in the per event HG saturation histogram
	}

	if( ( eventRepCountHG + eventRepCountLG ) > 0 )
	  hChRepetitionEvent->Fill( eventRepCountHG + eventRepCountLG );
	  // Fill in the global channel repetition histogram

	if( ( eventSatCountHG + eventSatCountLG ) > 0 )
	  hChSaturationEvent->Fill( eventSatCountHG + eventSatCountLG );
	  // Fill in the global channel saturation histogram
      
	clearEvent( CurrentEvent );
      }
    }
  }
}
