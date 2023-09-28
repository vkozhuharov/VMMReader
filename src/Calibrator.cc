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
#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TF1.h"

#include "Analyser.hh"


void Analyser::calFitter()
{
  std::cout << "Calibrating..." << std::endl;
  
  char pdfname[64];
  char pngname[64];

  TDirectory *dirCalGraphs = fOut->mkdir( "Calibration Graphs" );
  TDirectory *dirCalCanvases = fOut->mkdir( "Calibration Canvases" );

  for( auto graphVect: calVector )
  {
    graphVect->graph->SetMarkerStyle( 20 );
    graphVect->graph->SetMarkerSize( 0.5 );
    graphVect->graph->SetLineColor( 0 );
    
    graphVect->graph->GetHistogram()->GetXaxis()->SetLimits( 0.0, 1100. );
    graphVect->graph->GetHistogram()->GetYaxis()->SetRangeUser( 0.0, 1100. );

    hCalPointCount->Fill( graphVect->graph->GetN() );   // Fill in the point count histogram
    
    int xMin = graphVect->graph->GetXaxis()->GetXmin();
    int xMax = saturation;
    int nBins = xMax / fitBins;
    
    TH1F *hFitter = new TH1F( "hFitter", "XRange Histogram", nBins, xMin, xMax );

    for( int i = 0; i < graphVect->graph->GetN(); i++ )
    {
      double xVal, yVal;
      graphVect->graph->GetPoint( i, xVal, yVal );
      hFitter->Fill( xVal );
    }

    int maxBin = hFitter->GetMaximumBin();
    double FitLow = hFitter->GetXaxis()->GetBinLowEdge(maxBin) - fitExtendDown;
    double FitHigh = hFitter->GetXaxis()->GetBinUpEdge(maxBin) + fitExtendUp;

    TF1 *fit = new TF1( "LineFit", "[0]*x + [1]" );

    if( graphVect->graph->GetN() >= graphPointCountCut )
    {
      graphVect->graph->Fit( fit, "Q", "", FitLow, FitHigh );
      
      double p0 = fit->GetParameter( 0 );
      double p1 = fit->GetParameter( 1 );
      
      calChMap[std::make_pair( graphVect->vmm, graphVect->channel )]
	= std::make_pair( p0, p1 );
      
      hCalSlopes->Fill( p0 );   // Fill the slopes histogram
      hCalConsts->Fill( p1 );   // Fill the constants histogram
    }

    std::string vmm = std::to_string( graphVect->vmm );
    std::string channel = std::to_string( graphVect->channel );

    TCanvas *Calibrator = new TCanvas();

    graphVect->graph->Draw( "AP" );
    
    dirCalGraphs->cd();
    graphVect->graph->Write( ( "Graph-" + vmm + "-" + channel ).c_str() );

    dirCalCanvases->cd();
    Calibrator->Write( ( "Canvas-" + vmm + channel ).c_str() );

    fOut->cd();
    delete hFitter;
  }
}


void Analyser::drawOGGraphs()
{
  TDirectory *dirOriginalGraphs = fOut->mkdir( "Original Graphs" );
  
  for( auto graphVect: ogGraphs )
  {
    graphVect->graph->SetMarkerStyle( 20 );
    graphVect->graph->SetMarkerSize( 0.5 );
    graphVect->graph->SetLineColor( 0 );
    
    graphVect->graph->GetHistogram()->GetXaxis()->SetLimits( 0.0, 1100. );
    graphVect->graph->GetHistogram()->GetYaxis()->SetRangeUser( 0.0, 1100. );

    std::string vmm = std::to_string( graphVect->vmm );
    std::string channel = std::to_string( graphVect->channel );

    TCanvas *OGCanvas = new TCanvas();

    graphVect->graph->Draw( "AP" );
    
    dirOriginalGraphs->cd();
    graphVect->graph->Write( ( "Graph-" + vmm + "-" + channel ).c_str() );

    fOut->cd();
  }
}


std::pair< double, double > Analyser::getFitRange( TGraph *graph, int xMin, int xMax, int nBins )
{
  TH1F *hFitter = new TH1F( "hFitter", "XRange Histogram", nBins, xMin, xMax );

  for( int i = 0; i < graph->GetN(); i++ )
  {
    double xVal, yVal;
    graph->GetPoint( i, xVal, yVal );
    hFitter->Fill( xVal );
  }
  
  int maxBin = hFitter->GetMaximumBin();
  double fitLow = hFitter->GetXaxis()->GetBinLowEdge(maxBin) - fitExtendDown;
  double fitHigh = hFitter->GetXaxis()->GetBinUpEdge(maxBin) + fitExtendUp;

  delete hFitter;

  return std::make_pair( fitLow, fitHigh );
}


void Analyser::ogFitter( int vmm, int channel, TGraph *graph, double fitLow, double fitHigh )
{
  TF1 *fit = new TF1( "LineFit", "[0]*x + [1]" );

  if( graph->GetN() >= graphPointCountCut )
  {
    graph->Fit( fit, "Q", "", fitLow, fitHigh );
    
    double p0 = fit->GetParameter( 0 );
    double p1 = fit->GetParameter( 1 );

    hCalOGSlopes->Fill( p0 );   // Fill the slopes histogram
    hCalOGConsts->Fill( p1 );   // Fill the constants histogram

    constMap[std::make_pair( vmm, channel )] = p1;
  }
}

void Analyser::ogGrapher()
{
  std::cout << "Calibrating..." << std::endl;

  TDirectory *dirOGGraphs = fOut->mkdir( "OG Graphs" );

  for( auto graphVect: ogGraphs )
  {
    graphVect->graph->SetMarkerStyle( 20 );
    graphVect->graph->SetMarkerSize( 0.5 );
    graphVect->graph->SetLineColor( 0 );
    
    graphVect->graph->GetHistogram()->GetXaxis()->SetLimits( 0.0, 1100. );
    graphVect->graph->GetHistogram()->GetYaxis()->SetRangeUser( 0.0, 1100. );

    hCalOGPointCount->Fill( graphVect->graph->GetN() );   // Fill in the OG point count histogram
    
    int xMin = graphVect->graph->GetXaxis()->GetXmin();
    int xMax = saturation;
    int nBins = xMax / fitBins;
    
    std::pair< double, double > fitRanges = getFitRange( graphVect->graph, xMin, xMax, nBins );
 
    ogFitter( graphVect->vmm, graphVect->channel, graphVect->graph, fitRanges.first, fitRanges.second );

    std::string vmm = std::to_string( graphVect->vmm );
    std::string channel = std::to_string( graphVect->channel );

    TCanvas *Calibrator = new TCanvas();

    graphVect->graph->Draw( "AP" );
    
    dirOGGraphs->cd();
    graphVect->graph->Write( ( "Graph-" + vmm + "-" + channel ).c_str() );

    fOut->cd();
  }
}


void Analyser::graphCutter()
{
  for( auto graphVect: ogGraphs )
  {
    int nPoints = graphVect->graph->GetN();
    double* xPoints = graphVect->graph->GetX();
    double* yPoints = graphVect->graph->GetY();

    double meanSlope = hCalOGSlopes->GetMean();
    double meanConstant = hCalOGConsts->GetMean();

    auto it = ( constMap.find( std::make_pair( graphVect->vmm, graphVect->channel ) ) );
    double constant = it->second;

    TGraph *fitGraph = new TGraph();

    for( int i = 0; i < nPoints; i++ )
    {
      if( yPoints[i] != 1023 &&
	  yPoints[i] >= graphChargeDownCutHG &&
	  yPoints[i] >= meanSlope * xPoints[i] + constant - graphCutConstantOffset )
      {
	fitGraph->SetPoint( fitGraph->GetN(), xPoints[i], yPoints[i] );
      }
    }

    MapGraph2 *Filler = new MapGraph2;

    Filler->vmm = graphVect->vmm;
    Filler->channel = graphVect->channel;
    Filler->graph = fitGraph;

    calVector.push_back( Filler );
  }
}

void Analyser::finalFitter()
{
  TDirectory *dirCalGraphs = fOut->mkdir( "Calibration Graphs" );
  TDirectory *dirCalCanvases = fOut->mkdir( "Calibration Canvases" );

  for( auto graphVect: calVector )
  { 
    TF1 *fit = new TF1( "LineFit", "[0]*x + [1]" );
    
    if( graphVect->graph->GetN() >= graphPointCountCut )
    {
      hCalPointCount->Fill( graphVect->graph->GetN() );   // Fill in the final point count histogram
      
      graphVect->graph->Fit( fit, "Q" );
      
      double p0 = fit->GetParameter( 0 );
      double p1 = fit->GetParameter( 1 );
      
      calChMap[std::make_pair( graphVect->vmm, graphVect->channel )]
	= std::make_pair( p0, p1 );
      
      hCalSlopes->Fill( p0 );   // Fill the slopes histogram
      hCalConsts->Fill( p1 );   // Fill the constants histogram
    }
    
    std::string vmm = std::to_string( graphVect->vmm );
    std::string channel = std::to_string( graphVect->channel );
    
    TCanvas *Calibrator = new TCanvas();

    graphVect->graph->SetMarkerStyle( 20 );
    graphVect->graph->SetMarkerSize( 0.5 );
    graphVect->graph->SetLineColor( 0 );
    
    graphVect->graph->GetHistogram()->GetXaxis()->SetLimits( 0.0, 1100. );
    graphVect->graph->GetHistogram()->GetYaxis()->SetRangeUser( 0.0, 1100. );
    
    graphVect->graph->Draw( "AP" );
    
    dirCalGraphs->cd();
    graphVect->graph->Write( ( "Graph-" + vmm + "-" + channel ).c_str() );

    dirCalCanvases->cd();
    Calibrator->Write( ( "Canvas-" + vmm + channel ).c_str() );

    fOut->cd();
  }
}


void Analyser::calibrator()
{
  TFile *fTree = new TFile( "treeTemp.root", "READ" );
  TTree *tCalibrator;
  fTree->GetObject( "eventBuffer", tCalibrator );
  
  for( int entry = 0; entry < tCalibrator->GetEntries(); entry++ )
  { 
    int totalCalCharge = 0;
    int totalCalChargeHG = 0;
    int totalCalChargeLG = 0;

    std::vector< std::array< std::array< int, 64 >, 16 > > eventVect;
    
    tCalibrator->SetBranchAddress( "calVect", &eventVect );
    tCalibrator->GetEntry( entry );

    for( auto eventArr: eventVect )
    {
      for( int vmm = 0; vmm < 16; vmm++ )
      {
	for( int channel = 0; channel < 64; channel++ )
	{
	  if( eventArr[vmm][channel] == 0 )
	    continue;

	  else
	  {
	    int gain = gainSelector( channel );

	    if( gain == 0 )
	    {
	      totalCalChargeHG += eventArr[vmm][channel];
	      
	      if( channel >= 2 )
	      {
	      if( eventArr[vmm][channel-2] == 0 )
		totalCalCharge += eventArr[vmm][channel];
	      }
	    }
	    
	    else
	    {
	      totalCalChargeLG += eventArr[vmm][channel];
	      
	      if( eventArr[vmm][channel+2] >= calCutOff )
	      {
		auto it = calChMap.find( std::make_pair( vmm, channel ) );
		
		if( it != calChMap.end() )
		{
		  std::pair< double, double > fitRes = it->second;
		  double slope = fitRes.first;
		  double constant = fitRes.second;
		  
		  totalCalCharge += ( slope * eventArr[vmm][channel] + constant );
		}
		
		else
		  totalCalCharge += ( eventArr[vmm][channel+2] );
	      }
	      
	      else
		totalCalCharge += ( eventArr[vmm][channel+2] );
	    }
	  }
	}
      }
    
    if( totalCalCharge > 0 )
      hTotalChargeCal->Fill( totalCalCharge );   // Fill in the calibrated charge histogram
    
    if( totalCalChargeHG > 0 )
      hTotalChargeCalHG->Fill( totalCalChargeHG );   // Fill in the HG test histogram

    if( totalCalChargeLG > 0 )
      hTotalChargeCalLG->Fill( totalCalChargeLG );   // Fill in the LG test histogram
    }
  }
}


void Analyser::calibrator2( SRSData *srs )
{
  std::array< std::vector< SingleHit >, 3 > CurrentEvent;
  
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
    
    int Gain = gainSelector( CurrHit.channel );
    if( Gain == 0 )
      CurrHit.channelIsLG = false;
    else if( Gain == 1 )
      CurrHit.channelIsLG = true;
    
    if ( CurrentEvent[Gain].empty() ) { CurrentEvent[Gain].push_back( CurrHit ); }
    else
    { 
      SingleHit PrevHit;
      PrevHit = CurrentEvent[Gain].back();

      SingleHit FHit;
      FHit = CurrentEvent[Gain].front();

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
	int TCharge_Cal = 0;
	int TCharge_HG = 0;        // Stores totCharge for HG
	int TCharge_LG = 0;        // Stores totCharge for LG

	std::array< std::array< int, 64 >, 16 > Calibrator = {0};

	if( CurrentEvent[0].empty() == false )
	{
	  for( int i = 1; i < CurrentEvent[0].size() - 1; i++ )
	  {
	    if( Calibrator[CurrentEvent[0][i].vmm][CurrentEvent[0][i].channel] == 0 )
	    { 
	      Calibrator[CurrentEvent[0][i].vmm][CurrentEvent[0][i].channel]
		= CurrentEvent[0][i].charge;
	    }

	    else
	      continue;
	  }
	}
	
	if( CurrentEvent[1].empty() == false )
	{
	  for( int j = 1; j < CurrentEvent[1].size() - 1; j++ )
	  {
	    if( Calibrator[CurrentEvent[1][j].vmm][CurrentEvent[1][j].channel] == 0 )
	    {
	      Calibrator[CurrentEvent[1][j].vmm][CurrentEvent[1][j].channel]
		= CurrentEvent[1][j].charge;
	    }

	    else
	      continue;
	  }
	}
	
	for( int vmm = 0; vmm < 16; vmm++ )
	{
	  for( int channel = 0; channel < 64; channel++ )
	  {
	    if( Calibrator[vmm][channel] == 0 )
	      continue;

	    else
	    {
	      int gain = gainSelector( channel );

	      if( gain == 0 )
	      {
		TCharge_HG += Calibrator[vmm][channel];
	      
		if( channel >= 2 )
		{
		  if( Calibrator[vmm][channel-2] == 0 )
		    TCharge_Cal += Calibrator[vmm][channel];
		}
	      }
	    
	      else
	      {
		TCharge_LG += Calibrator[vmm][channel];
	      
		if( Calibrator[vmm][channel+2] >= calCutOff )
		{
		  auto it = calChMap.find( std::make_pair( vmm, channel ) );
		
		  if( it != calChMap.end() )
		  {
		    std::pair< double, double > fitRes = it->second;
		    double slope = fitRes.first;
		    double constant = fitRes.second;
		  
		    TCharge_Cal += ( slope * Calibrator[vmm][channel] + constant );
		  }
		
		  else
		    TCharge_Cal += ( Calibrator[vmm][channel+2] );
		}
	      
		else
		  TCharge_Cal += ( Calibrator[vmm][channel+2] );
	      }
	    }
	  }
	}
      
	if( TCharge_Cal > 0 )
	  hTotalChargeCal->Fill( TCharge_Cal );   // Fill in the calibrated charge histogram
	
	if( TCharge_HG > 0 )
	  hTotalChargeCalHG->Fill( TCharge_HG );   // Fill in the HG test histogram
	
	if( TCharge_LG > 0 )
	  hTotalChargeCalLG->Fill( TCharge_LG );   // Fill in the LG test histogram

	clearEvent( CurrentEvent );
      }
    }
  }
}
