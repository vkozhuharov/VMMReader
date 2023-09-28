#ifndef _PCAPNG_READER_
#define _PCAPNG_READER_

#include <netinet/ip.h>
#include <netinet/udp.h>
#include <cassert>
#include <cinttypes>
#include <cstring>
#include <string>
#include <pcap.h>


class PCAPNGReader {
public:
  PCAPNGReader( std::string FileName );
  ~PCAPNGReader();
  
  int open();
  int read( char *Buffer, size_t BufferSize );

  int getStats();
  void printStats();
  void printPacket( unsigned char *Data, size_t PayloadLength );
  
  struct stats_t
  {
    uint64_t PacketsTotal;
    uint64_t PacketsTruncated;
    uint64_t BytesTotal;
    uint64_t PacketsNoMatch;
    uint64_t IpProtoUDP;
  } Stats;
  
private:
  const char *FilterUdp = "ip and udp";
  struct bpf_program PcapFilter;
  int validatePacket( pcap_pkthdr *Header, const unsigned char *Data );
  std::string FileName;
  pcap_t *PcapHandle{ nullptr };
};


#endif
