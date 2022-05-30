
#ifndef _UTILS_H_
#define _UTILS_H_

uint16_t hwCalcCRC16(const uint8_t *pData, const size_t len, uint16_t seed);
uint16_t calcCRC16(const uint8_t* pData, const size_t len, uint16_t seed);

#define USE_INLINE_MACRO  (1)

#if USE_INLINE_MACRO
#define  makeIP(ip1, ip2, ip3, ip4)  ((ip1 << 24) + (ip2 << 16) + (ip3 << 8) + ip4)
#define  getIPField(ip, idx)         ((uint8_t)((ip >> (8 * (4 - idx))) & 0xFF))
#else
uint32_t makeIP(uint8_t ip1, uint8_t ip2, uint8_t ip3, uint8_t ip4);
uint8_t getIPField(uint32_t ip, uint8_t idx);
#endif

#endif /* end of _UTILS_H_ */
