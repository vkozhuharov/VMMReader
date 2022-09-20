#include "PCAPNGReader.hh"
#include <iostream>

const int ETHERNET_HEADER_SIZE = 14;
const int IP_HEADER_SIZE = 20;
const int UDP_HEADER_SIZE = 8;

const int IP_HEADR_OFFSET = ETHERNET_HEADER_SIZE;
const int UDP_HEADER_OFFSET = IP_HEADR_OFFSET + IP_HEADER_SIZE;
const int UDP_DATA_OFFSET = UDP_HEADER_OFFSET + UDP_HEADER_SIZE;

PCAPNGReader::PCAPNGReader(std::string FileName)
  : FileName(FileName) {
  memset(&Stats, 0, sizeof(stats_t));
}


PCAPNGReader::~PCAPNGReader() {
  if (PcapHandle != NULL ) {
    pcap_close(PcapHandle);
  }
}


int PCAPNGReader::open() {
  char ErrorBuffer[PCAP_ERRBUF_SIZE];
  PcapHandle = pcap_open_offline(FileName.c_str(), ErrorBuffer);
  if (PcapHandle == nullptr) {
    return -1;
  }

  if (pcap_compile(PcapHandle, &PcapFilter, FilterUdp, 1, PCAP_NETMASK_UNKNOWN) == -1) {
    return -1;
  }

  return 0;
}

int PCAPNGReader::validatePacket(pcap_pkthdr *Header, const unsigned char *Data) {

  Stats.PacketsTotal++; /**< total packets in pcap file */
  Stats.BytesTotal += Header->len;

  if (Header->len != Header->caplen) {
    Stats.PacketsTruncated++;
    return 0;
  }

  if (pcap_offline_filter(&PcapFilter, Header, Data) == 0) {
    Stats.PacketsNoMatch++;
    return 0;
  }

  Stats.IpProtoUDP++;
  assert(Stats.PacketsTotal == Stats.PacketsTruncated+ Stats.PacketsNoMatch + Stats.IpProtoUDP);
  assert(Header->len > ETHERNET_HEADER_SIZE + IP_HEADER_SIZE + UDP_HEADER_SIZE);

  udphdr *udp = (udphdr *)&Data[UDP_HEADER_OFFSET];
  uint16_t UdpLen = htons(udp->uh_ulen);

  assert(UdpLen >= UDP_HEADER_SIZE);

 
  return UdpLen;
}

int PCAPNGReader::read(char *Buffer, size_t BufferSize) {
  if (PcapHandle == nullptr) {
    return -1;
  }

  pcap_pkthdr *Header;
  const unsigned char *Data;
  int ret = pcap_next_ex(PcapHandle, &Header, &Data);
  if (ret < 0) {
    return -1;
  }
  //  unsigned char *ptr = Data;
  
  //  printPacket(ptr,10);
  
  int UdpDataLength;
  if ((UdpDataLength = validatePacket(Header, Data)) <= 0) {
    //    std::cout << "UdpDataLength:  " << UdpDataLength << std::endl;
    return UdpDataLength;
  }

  auto DataLength = std::min((size_t)(UdpDataLength - UDP_HEADER_SIZE), BufferSize);
  std::memcpy(Buffer, &Data[UDP_DATA_OFFSET], DataLength);

  return DataLength;
}




int PCAPNGReader::getStats() {
  if (PcapHandle == nullptr) {
    return -1;
  }

  while (true) {
    int RetVal;
    pcap_pkthdr *Header;
    const unsigned char *Data;
    if ((RetVal = pcap_next_ex(PcapHandle, &Header, &Data))  < 0) {
      break;
    }
    validatePacket(Header, Data);
  }
  return 0;
}

void PCAPNGReader::printPacket(unsigned char *Data, size_t Size) {
  for (unsigned int i = 0; i < Size; i++) {
    if ((i % 16) == 0 && i != 0) {
      printf("\n");
    }
    //    printf("%.2x ", Data[i]);
    printf("% 4d ", Data[i]);
  }
  printf("\n");
}


void PCAPNGReader::printStats() {
  printf("Total packets        %d\n", Stats.PacketsTotal);
  printf("Truncated packets    %d\n", Stats.PacketsTruncated);
  printf("  ipproto UDP        %d\n", Stats.IpProtoUDP);
  printf("  other              %d\n", Stats.PacketsNoMatch);
  printf("Total bytes          %d\n", Stats.BytesTotal);
}
