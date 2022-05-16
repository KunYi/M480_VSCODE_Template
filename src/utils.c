#include <stdint.h>
#include <stdio.h>

/*
  CRC16-CCIT pre-calc table for nibble
  table[0] = 0x0000,
  table[1] = 0x0000 + 0x1021,
  table[2] = 0x1021 + 0x1021 = 0x2042,
  table[3] = 0x2042 + 0x1021 = 0x3063,
  ...
  table[15] = 0xF1EF
*/
static const uint16_t ccit16_tab[16] = {
    0x0000, 0x1021, 0x2042, 0x3063,
    0x4084, 0x50a5, 0x60c6, 0x70e7,
    0x8108, 0x9129, 0xA14A, 0xB16B,
    0xC18C, 0xD1AD, 0xE1CE, 0xF1EF
};

static const uint8_t rtab16[] = {
    0,  8, 4, 12,
    2, 10, 6, 14,
    1,  9, 5, 13,
    3, 11, 7, 15
};

static uint8_t reverseU8(const uint8_t data) {
    return (uint8_t)(
           (rtab16[data & 0xF] << 4) |
            rtab16[data >> 4 & 0xF]);
}

static uint16_t reverseU16(const uint16_t data) {
    return (uint16_t)(
        (rtab16[data & 0xF] << 12) |
        (rtab16[data >> 4 & 0xF] << 8) |
        (rtab16[data >> 8 & 0xF] << 4) |
         rtab16[data >> 12 & 0xF]);
}

uint16_t calcCRC16(const uint8_t* pData, const size_t len, uint16_t prev)
{
    uint8_t tmp;
    uint16_t ret=prev;

    for (unsigned i = 0; i < len; i++) {
        tmp =  reverseU8(pData[i]);
        ret = (uint16_t)(ret << 4 ^ ccit16_tab[(ret >> 12) ^ (tmp >> 4)]);
        ret = (uint16_t)(ret << 4 ^ ccit16_tab[(ret >> 12) ^ (tmp & 0xF)]);
    }
    return reverseU16(ret) ^ 0x0000;
}

uint32_t makeIP(uint8_t ip1, uint8_t ip2, uint8_t ip3, uint8_t ip4)
{
  return (ip1 << 24) + (ip2 << 16) + (ip3 << 8) + ip4;
}

uint8_t getIPField(uint32_t ip, uint8_t idx)
{
  return ((ip >> (8 * (4 - idx))) & 0xFF);
}
