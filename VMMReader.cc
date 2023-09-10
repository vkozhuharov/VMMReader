#include "PCAPNGReader.hh"
#include "SRSData.hh"
#include "Analyser.hh"
#include <iostream>
#include <string>
#include <chrono>


const int BUFSIZE = 100000;

int main( int argc, char * argv[] )
{
  for ( int a = 1; a < argc; ++a )
  {
    auto start = std::chrono::high_resolution_clock::now();
    
    char buf[BUFSIZE];
    std::string filename = argv[a];
    PCAPNGReader *PCAPStream = new PCAPNGReader( filename );
    SRSData *srs = new SRSData;
    Analyser *ana = new Analyser();
    
    
    if( PCAPStream->open() == 0 )
      std::cout << argv[a] << " opened successfully." << std::endl;
  
    else
    {
      std::cout << "Cannot open file." << std::endl;
      exit(0);
    }

    // ana->ClearTestFile();
    
    int dataSize = 0;
    int count = 1;
    float ptimetot = 0;
    float timetot = 0;
    while( ( dataSize = PCAPStream->read( buf, BUFSIZE ) ) != -1 )
    {
      // std::cout << "Found packet with size:   "<< dataSize << std::endl;
      if( dataSize != 8968 ) continue;
      
      srs->setDataBuffer(buf,dataSize);
      int res = srs->decode();
      if ( res == -1 ) continue;

      /*
      std::cout << "FrameCounter: " << srs->hdr.frameCounter << std::endl;
      // std::cout << "data ID     : " << srs->hdr.dataId << std::endl;
      printf("Data id: %x\n",srs->hdr.dataId );
      std::cout << "DataSize without header:    " << 1.*srs->dataSize << std::endl;
      std::cout << "Number of hits in the event:     " << srs->getHits().size() << std::endl;
      */
      
      // std::cout << "Charge in channel: " << srs->hits[ih]->adc << std::endl;

      auto pstart = std::chrono::high_resolution_clock::now();
      ana->process( srs );
      auto pstop = std::chrono::high_resolution_clock::now();
      
      auto ptime = std::chrono::duration_cast<std::chrono::microseconds>( pstop - pstart ).count();
      timetot += ptime;

      if( count % 1000 == 0 )
      {
	std::cout << "\r" << "Packets = " << count << ". Average packet analysis time = " << timetot/count << " us." << std::flush;
      }
      count++;
    }

    auto stop = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::seconds>( stop - start ).count();
    std::cout << std::endl << "Final packet = " << count << ". Full analysis time = " << time << " s." << std::endl << std::endl;

    // ana->plotting();
    std::cout << std::endl;
    // PCAPStream->printStats();
    if( ana ) delete ana;
    if( srs ) delete srs;
    if( PCAPStream ) delete PCAPStream;
  }
}
