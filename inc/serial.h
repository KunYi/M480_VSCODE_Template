
#ifndef _SERIAL_H_
#define _SERIAL_H_

#ifdef __cplusplus
extern "C" {
#endif

uint32_t convPortDataLen(const uint32_t v);
uint32_t convPortParity(const uint32_t v);
uint32_t convPortStopBits(const uint32_t v);


#ifdef __cplusplus
}
#endif

#endif /* end of #ifndef _SERIAL_H_ */
