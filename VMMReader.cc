#include "PCAPNGReader.hh"
#include <iostream>
#include "SRSData.hh"
#include "Analyser.hh"


const int BUFSIZE=100000;

int main(){
  char buf[BUFSIZE];
  
  PCAPNGReader *PCAPStream = new PCAPNGReader("DumpData.pcapng");
  SRSData *srs = new SRSData;
  Analyser *ana = new Analyser;

  
  if(PCAPStream->open() == 0) {
    std::cout << "File opened successfully" << std::endl;
  } else {
    std::cout << "Cannot open file" << std::endl;
    exit(0);
  }
  
  int dataSize=0;
  while( (dataSize = PCAPStream->read(buf, BUFSIZE)) != -1) {
    //    std::cout << "Found packet with size:   "<< dataSize << std::endl;
    if(dataSize != 8968) continue;
    
    srs->setDataBuffer(buf,dataSize);
    int res = srs->decode();
    if (res == -1) continue;
    
    std::cout << "FrameCounter: " << srs->hdr.frameCounter << std::endl;
    //    std::cout << "data ID     : " << srs->hdr.dataId << std::endl;
    printf("Data id: %x\n",srs->hdr.dataId );
    std::cout << "DataSize without header:  " << srs->dataSize << std::endl;
    std::cout << "Number of hits in the event:    " << srs->getHits().size() << std::endl;

    ana->process(srs);
    
  }
  
  //PCAPStream->printStats();
  if(ana) delete ana;
  if(srs) delete srs;
  if (PCAPStream) delete PCAPStream;
}
