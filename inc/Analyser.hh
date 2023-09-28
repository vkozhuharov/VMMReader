#ifndef _ANALYSER_H_
#define _ANALYSER_H_

#include "TDirectory.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"

#include "SRSData.hh"

class Analyser
{
public:
  int saturation = 1023;   // Used to impose artificial saturation
  int calCutOff = 900;     // Determines at what point the calibration takes effect

  int graphHitCut = 10;             // Minimum event hits limit for points to plot on the graphs
  int graphChargeDownCutHG = 200;   // HG minimum limit for points to plot on the graphs
  int graphChargeRatioCut = 10;     // Removes points with HG / LG ratio beneath this value
  int graphPointCountCut = 100;
  int graphCutConstantOffset = 250;
  
  int fitBins = 30;         // Bin size for the histogram that determines calibration fit position
  int fitExtendDown = 20;   // Down extension from the lower edge of the max points bin
  int fitExtendUp = 20;     // Up extension from the lower edge of the max points bin

  std::array< double, 2 > enGeVHG = { 1., 1. };    // Linearity coefficients for the HG GeV histogram
  std::array< double, 2 > enGeVLG = { 1., 1. };    // Linearity coefficients for the LG GeV histogram
  std::array< double, 2 > enGeVCal = { 1., 1. };   // Linearity coefficients for the calibrated GeV histogram
  
  std::vector< int > VMMs { 0, 1, 4, 5, 8, 9, 12, 13 };   // Define VMMs to look for. Does nothing now.

  std::vector< int > HGChannels { 63, 62, 59, 58, 55, 54, 51, 50, 47, 46, 43, 42,   // HG Channel mapping
    39, 38, 35, 34, 31, 30, 27, 26, 23, 22, 19, 18, 15, 14, 11, 10, 7, 6, 3, 2 };

  std::vector< int > LGChannels { 61, 60, 57, 56, 53, 52, 49, 48, 45, 44, 41, 40,   // LG Channel mapping
    37, 36, 33, 32, 29, 28, 25, 24, 21, 20, 17, 16, 13, 12, 9, 8, 5, 4, 1, 0 };

  std::array< std::array< int, 64 >, 2 > CH_Mapper;   // CH_Mapper[0] is HG, CH_Mapper[1] is LG

  struct MapGraph { TGraph *graph; };
  
  typedef struct
  {
    int vmm;
    int channel;
    TGraph *graph;
  } MapGraph2;

  // std::map< std::pair< int, int >, MapGraph > calMap;   // Processing map for per channel calibration
  // std::vector< MapGraph2* > calVectorMid;               // Vector to store the calibration graphs
  std::vector< MapGraph2* > calVector;                     // Vector to store the calibration graphs

  std::map< std::pair< int, int >, MapGraph > ogMap;
  std::vector< MapGraph2* > ogGraphs;                  // Vector to store the completely uncut graphs

  std::map< std::pair< int, int >, double > constMap;
  // std::map< std::pair< int, int >, double > finalConstMap;

  typedef struct
  {
    int vmm;
    int channel;
    int chargeHG;
    int chargeLG;
  } toCalibrate;

  std::map< std::pair< int, int >, std::pair< double, double > > calChMap;   // Map to the calibration fits

  typedef struct
  {
    int channelIsLG;
    int vmm;
    int channel;
    int charge;
    int bcid;
    int tdo;
    int overflow;
    double ts_ns;
  } SingleHit;
  
  std::array< std::vector< SingleHit >, 3 > CurrentEvent;          // 0 -> HG, 1 -> LG, 2 -> errors
  // std::vector< std::array< std::array< int, 64 >, 16 > > calVect;   // Stores Calibrators for TTree filling
  
  
public:
  Analyser();            // Initiates the histograms and the output file
  Analyser( char *s );   // Not used as of now
  ~Analyser();           // Writes the histograms and the file and exits
  
  void init();                           // Maps the LG and HG channels to an array
  void clearTestFile();                  // Not used as of now
  void clearTreeFile();
  void clearEvent( auto &EventArray );   // Empties the Event array after processing
  
  int elemIndex( auto array, int vmm );   // Get the index of an array element
  int fForward( SRSData *srs, int ih );   // Allows for skipping bad hits
  int gainSelector( int ch );             // Allows to differentiate HG and LG

  int eventContinuity( SingleHit CurrHit, SRSData *srs, int ih, int nhits );
  // Allow for bypassing "entangled" and "split" events. Compares hit to previous hit
  
  int eventContinuity2( SingleHit CurrHit, SRSData *srs, int ih, int nhits );
  // Allow for bypassing "entangled" and "split" events. Compares hit to first hit

  double calcChipTime( SRSData *srs, int ih );    // Calculate Chip timestamp
  double calcFECTime( SRSData *srs, int ih );     // Calculate FEC timestamp
  double calcTimeStamp( SRSData *srs, int ih );   // Calculate high-order timestamp

  void drawHitMap();
  void process( SRSData *srs, int batchCount );   // Does the actual analysis
  void drawOGGraphs();
  void calFitter();               // Draws, fits and saves the calibration graphs
  void calibrator();              // Does the HG / LG calibration
  void calibrator2( SRSData *srs );

  
  void finalFitter();
  void ogGrapher();
  // void graphCutter();
  void graphCutter();
  void ogFitter( int vmm, int channel, TGraph *graph, double fitlow, double fitHigh );

  std::pair< double, double > getFitRange( TGraph *graph, int xMin, int xMax, int nBins );

  SingleHit getHitData( SRSData *srs, int ih );   // Quick access to hit data

  std::map< std::pair< int, int >, std::pair< int, int > >   // Load in the mapping
  load_table( const std::string &filename );

  std::map< std::pair< int, int >, std::array< int, 3 > >   // Load in the mapping
  mapLoader( const std::string &filename );

  
public:
  TFile *fOut;   // ROOT output file
  
  TH1F *hChActivation;          // Number of activations for each channel
  TH1F *hChRepetition;          // Displays double activations of the same channels in a single event
  TH1F *hChRepetitionHG;        // Displays double activations of the same LG channels in a single event
  TH1F *hChRepetitionLG;        // Displays double activations of the same LG channels in a single event
  TH1F *hChRepetitionEvent;     // Counts double activations of the same channels per event
  TH1F *hChRepetitionEventHG;   // Counts double activations of the same HG channels per event
  TH1F *hChRepetitionEventLG;   // Counts double activations of the same LG channels per event

  TH2F *hOutModuleOccupancy[10];   // Hit distribution in the outer modules
  TH2F *hCenterModuleOccupancy;   // Hit distribution in the central module
  // TH2F *hChOccupancy;
  // TH2F *hEventChOccupancy;
  // TH2F *hChChargeOccupancy;
  // TH2F *hEventChChargeOccupancy;

  // TH2F *hHitOccupancy;
  // TH2F *hHitChargeOccupancy;
  // TH2F *hFilteredHitChargeOccupancy;
  // TH2F *hFilteredHitChargeOccupancy1;
  // TH2F *hSaturationMap;
  
  TH1F *hTotalChargeHG;      // HG total charge
  TH1F *hTotalChargeHG_F;    // HG total charge with all "events" of less than 10 hits filtered
  TH1F *hTotalChargeLG;      // LG total charge
  TH1F *hTotalChargeLG_F;    // LG total charge with all "events" of less than 10 hits filtered
  TH1F *hTotalChargeCal;     // total charge after calibration ( HG + LG )
  TH1F *hTotalChargeCalHG;   // Test histogram for the HG charges when filling the calibrated histogram
  TH1F *hTotalChargeCalLG;   // Test histogram for the LG charges when filling the calibrated histogram

  TH1F *hTotalChargeHG_GeV;    // HG total charge in GeV
  TH1F *hTotalChargeLG_GeV;    // LG total charge in GeV
  TH1F *hTotalChargeCal_GeV;   // Calibrated total charge in GeV

  TH1F *hChSaturation;          // Saturated channels for HG + LG
  TH1F *hChSaturationHG;        // Saturated channels for HG
  TH1F *hChSaturationLG;        // Saturated channels for LG
  TH1F *hChSaturationEvent;     // Count of saturated channels per event for HG + LG
  TH1F *hChSaturationEventHG;   // Count of saturated channels per event for HG 
  TH1F *hChSaturationEventLG;   // Count of saturated channels per event for LG

  TH1F *hCalOGSlopes;        // Holds the values of the slopes for all channels
  TH1F *hCalMidSlopes;       // Holds the values of the slopes for all channels
  TH1F *hCalSlopes;          // Holds the values of the slopes for all channels
  TH1F *hCalOGConsts;        // Holds the values of the constants for all channels
  TH1F *hCalMidConsts;       // Holds the values of the constants for all channels
  TH1F *hCalConsts;          // Holds the values of the constants for all channels
  TH1F *hCalOGPointCount;    // Holds the numbers of points in the graphs for each channel
  TH1F *hCalMidPointCount;   // Holds the numbers of points in the graphs for each channel
  TH1F *hCalPointCount;      // Holds the numbers of points in the graphs for each channel

  TH1F *hSeparator[10];   // Makes things look better in TBrowser

  
public:
  int chmap[16][64];
  char name[64];

  struct Packet_Events
  {
    int NumHits;
    int TotCharge;
    int Saturated;
  } Pack_Buffer;
  
  std::vector< Packet_Events > Packet;
};
#endif
