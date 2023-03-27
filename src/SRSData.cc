#include "SRSData.hh"
#include <arpa/inet.h>
#include <bits/stdc++.h>
#include "stdio.h"



int Gray2Bin( int n )
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

  VMM3Data *hit = new VMM3Data; //new hit
  
  uint32_t data1 = htonl( i32buf[0] );
  uint16_t data2 = htons( i16buf[2] ); //first 4 bytes are for data1

    
  int dataflag = ( data2 >> 15 ) & 0x1;
  if( dataflag )
  {
    hit->overThreshold = ( data2 >> 14 ) & 0x1;
    hit->chno = ( data2 >> 8 ) & 0x3F;
    hit->tdc = data2 & 0xff;
    hit->vmmid = ( data1 >> 22 ) & 0x1F;
    hit->triggerOffset = ( data1 >> 27 ) & 0x1F;
    hit->adc = ( data1 >> 12 ) & 0x3FF;
    hit->bcid = Gray2Bin( data1 & 0xFFF );
    
    hits.push_back(hit);
  } 

  return 6;
}



int SRSData::decode()
{
  clearHits();
 
  decodeHdr( dataBuffer );
  char *dataPtr = dataBuffer;
  
  int pos = SRSHeaderSize; //Starting position after the SRS header

  //  decodeHit((void *) &(dataPtr[pos]));
  
  //  decodeHit(&(dataPtr[pos]));
  
  while ( pos < bufSize )
  {
    int hitSize = decodeHit( &( dataPtr[pos] ) );
    //    printf("%d\n", hitSize);
    
    if( hitSize == -1 ) return -1; //stop the decoding and go to the next
    pos += hitSize;
  }
  
  return 0;
}
