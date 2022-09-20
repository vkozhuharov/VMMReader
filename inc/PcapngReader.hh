#include <netinet/ip.h>
#include <netinet/udp.h>
#include <cassert>
#include <cinttypes>
#include <cstring>


const int ETHERNET_HEADER_SIZE = 14;
const int IP_HEADER_SIZE = 20;
const int UDP_HEADER_SIZE = 8;

const int IP_HEADR_OFFSET = ETHERNET_HEADER_SIZE;
const int UDP_HEADER_OFFSET = IP_HEADR_OFFSET + IP_HEADER_SIZE;
const int UDP_DATA_OFFSET = UDP_HEADER_OFFSET + UDP_HEADER_SIZE;

