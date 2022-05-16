#include <stdint.h>

uint32_t makeIP(uint8_t ip1, uint8_t ip2, uint8_t ip3, uint8_t ip4)
{
  return (ip1 << 24) + (ip2 << 16) + (ip3 << 8) + ip4;
}

uint8_t getIPField(uint32_t ip, uint8_t idx)
{
  return ((ip >> (8 * (4 - idx))) & 0xFF);
}
