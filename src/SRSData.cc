#include "SRSData.hh"
#include <arpa/inet.h>
#include <bits/stdc++.h>
#include "stdio.h"


int SRSData::gray2Bin( int n )
{
  int res = n;
  while ( n > 0)
  {
    n >>= 1;
    res ^= n;
  }
  
  return res;
}


void SRSData::setDataBuffer( void *ptr, int size )
{
  dataBuffer = ptr;
  bufSize = size;
}


int SRSData::decodeHdr( void *dataBuffer )
{
  struct SRSHeader * tmphdr =  dataBuffer;
  hdr.frameCounter = ntohl( tmphdr->frameCounter );
  hdr.dataId = ntohl( tmphdr->dataId );
  
  if ( ( hdr.dataId & 0xffffff00 ) != 0x564d3300 )
  {
    return -1; //unknown data
  }

  fecId = ( hdr.dataId >> 4 ) & 0x0f;
  hdr.udpTimeStamp =  ntohl( tmphdr->udpTimeStamp );
  
  dataSize = bufSize - SRSHeaderSize;
  return 0;
}


void SRSData::clearHits()
{
  for ( int i = 0; i < hits.size(); i++ )
  {
    if( hits[i] ) delete hits[i];  
  }
  
  hits.clear();
}


int SRSData::decodeHit( void *buf )
{
  char *cbuf = buf;
  uint32_t *i32buf = buf;
  uint16_t *i16buf = buf;

  VMM3Data *hit = new VMM3Data; // new hit
  VMM3FEC *marker = new VMM3FEC; // new marker
  
  uint32_t data1 = htonl( i32buf[0] );
  uint16_t data2 = htons( i16buf[2] ); // first 4 bytes are for data1

    
  int dataflag = ( data2 >> 15 ) & 0x1;
  if( dataflag )
  {
    hit->bcid = gray2Bin( data1 & 0xFFF );
    hit->adc = ( data1 >> 12 ) & 0x3FF;
    hit->tdc = data2 & 0xff;
    hit->chno = ( data2 >> 8 ) & 0x3F;
    hit->overThreshold = ( data2 >> 14 ) & 0x1;
    hit->vmmid = ( data1 >> 22 ) & 0x1F;
    hit->triggerOffset = ( data1 >> 27 ) & 0x1F;
    hit->hasDataMarker = true;

    hits.push_back( hit );

    /*
    std::cout << "< Event >" << "   VMM: " << 1*hit->vmmid << "   CH: " << 1*hit->chno
	      << "   BCID: " << 1*hit->bcid << "   TDC: " << 1*hit->tdc
	      << "   Offset: " << 1*hit->triggerOffset << "   OT: " << 1*hit->overThreshold
	      << "   ADC: " << 1*hit->adc << std::endl << std::endl;
    */
  }
  
  else
  {
    uint8_t vmmid = ( data2 >> 10 ) & 0x1F;

    uint64_t timestamp_lower_10bit = data2 & 0x03FF;
    uint64_t timestamp_upper_32bit = data1;
    
    uint64_t timestamp_42bit = ( timestamp_upper_32bit << 10 )
        + timestamp_lower_10bit;

    hit->vmmid = vmmid;
    // hit->ts_lower10bit = timestamp_lower_10bit;
    // hit->ts_upper32bit = timestamp_upper_32bit;
    hit->fecTimeStamp = timestamp_42bit;
    hit->hasDataMarker = false;

    hits.push_back( hit );

    /*
    std::cout << "< Marker >" << "   VMM: " << 1.*marker->vmmid
	      << "   FEC_TS: " << 1.*marker->ts_42bit
	      << std::endl << std::endl;
    */
  }
  
  return 6;
}



int SRSData::decode()
{
  clearHits();
 
  decodeHdr( dataBuffer );
  char *dataPtr = dataBuffer;
  
  int pos = SRSHeaderSize; // Starting position after the SRS header

  //  decodeHit((void *) &(dataPtr[pos]));
  
  //  decodeHit(&(dataPtr[pos]));
  
  while ( pos < bufSize )
  {
    int hitSize = decodeHit( &( dataPtr[pos] ) );
    //    printf("%d\n", hitSize);
    
    if( hitSize == -1 ) return -1; // stop the decoding and go to the next
    pos += hitSize;
  }
  
  return 0;
}
