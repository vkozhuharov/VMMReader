#ifndef _SRS_DATA_H_
#define _SRS_DATA_H_

#include <cstdint>
#include <vector>

#define SRSHeaderSize 16 //4 words, 4 bites each
#define HitAndMarkerSize 6 //6 bites 
#define Data1Size 4


struct SRSHeader
{
  uint32_t frameCounter;   /// frame counter packet field
  uint32_t dataId;         /// data type identifier packet field + ID of the FEC card (0-255)
  uint32_t udpTimeStamp;   /// Transmission time for UDP packet
  uint32_t offsetOverflow; /// indicates if marker for vmm was sent in last frame
};


/// Data related to a single Hit
struct VMM3Data
{
  uint64_t fecTimeStamp; /// 42 bits can change within a packet so must be here
  uint16_t bcid;         /// 12 bit - bcid after graydecode
  uint16_t adc;          /// 10 bit - adc value from vmm readout
  uint8_t tdc;           ///  8 bit - tdc value from vmm readout
  uint8_t chno;          ///  6 bit - channel number from readout
  uint8_t overThreshold; ///  1 bit - over threshold flag for channel from readout
  uint8_t vmmid;         ///  5 bit - asic identifier - unique id per fec 0 - 15
  uint8_t triggerOffset; ///  5 bit
  bool hasDataMarker;    ///
};


struct VMM3FEC
{
  uint8_t vmmid;
  uint64_t ts_lower10bit;
  uint64_t ts_upper32bit;
  uint64_t ts_42bit;
};


class SRSData
{
public:
  SRSData(){;};
  ~SRSData(){;};

  void setDataBuffer( void *, int );
  int gray2Bin( int n );
  int decode();
  int decodeHdr( void * );
  int decodeHit( void * );
  double calculateChipTime( int bcid, int tdc );
  double calculateFECTime();
  double calculateHOTS();
  void clearHits();
  std::vector < VMM3Data * > &getHits(){ return hits; };
  
public:
  void *dataBuffer;
  int bufSize;
  int dataSize;
  
  struct SRSHeader hdr;
  uint8_t fecId;

  std::vector < VMM3Data * > hits;
  // std::vector < VMM3FEC * > markers;
  uint64_t finalFEC_TS;
};

#endif
