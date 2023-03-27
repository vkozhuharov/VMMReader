#include "PCAPNGReader.hh"
#include "SRSData.hh"
#include "Analyser.hh"
#include <iostream>
#include <string>


const int BUFSIZE = 100000;

int main(int argc, char * argv[])
{
  for ( int a = 1; a < argc; ++a )
  {
    char buf[BUFSIZE];
    std::string dir = "./Data/SPS_Nov2022/";
    std::string filename = argv[a];
    // std::cout<<filename<<std::endl<<(dir+filename)<<std::endl;
    
    PCAPNGReader *PCAPStream = new PCAPNGReader( dir + filename );
    // PCAPNGReader *PCAPStream = new PCAPNGReader("Muon3VMM.pcapng");
    SRSData *srs = new SRSData;
    Analyser *ana = new Analyser();
    
    
    if( PCAPStream->open() == 0 )
    {
      std::cout << argv[a] << " opened successfully." << std::endl;
    }
  
    else
    {
      std::cout << "Cannot open file." << std::endl;
      exit(0);
    }

    // ana->ClearTestFile();
    
    int dataSize = 0;
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
      
      ana->process( srs );
    }
  
    // ana->plotting();
    // PCAPStream->printStats();
    if( ana ) delete ana;
    if( srs ) delete srs;
    if( PCAPStream ) delete PCAPStream;
  }
}
