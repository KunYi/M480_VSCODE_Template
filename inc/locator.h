#ifndef _LOCATOR_H_
#define _LOCATOR_H_

#define LOCATOR_PORT  (23)

#define TAG0            ('U')
#define TAG1            ('C')
#define DISCOVERY_CMD   (0xFE)
#define DISCOVERY_VER   (0)
#define CRC0            (0xFF)
#define CRC1            (0xFF)

struct UDP_DISCOVERY_COMMAND {
    uint8_t   tag[2];
    uint8_t   length;
    uint8_t   command;
    uint8_t   pktver;
    uint8_t   reserv;
    uint16_t  crc16;
} __attribute__((packed));

struct UDP_DISCOVERY_RESPONSE {
    uint8_t   tag[2];         /*  Offset:  0, Size:  2 */
    uint8_t   length;         /*  Offset:  2, Size:  1 */
    uint8_t   command;        /*  Offset:  3, Size:  1 */
    uint8_t   pktver;         /*  Offset:  4, Size:  1 */
    uint8_t   reserv;         /*  Offset:  5, Size:  1 */
    uint16_t  type;           /*  Offset:  6, Size:  2 */
    uint16_t  model;          /*  Offset:  8, Size:  2 */
    uint16_t  hwrev;          /*  Offset: 10, Size:  2 */
    uint32_t  fwver;          /*  Offset: 12, Size:  4 */
    uint32_t  ip4;            /*  Offset: 16, Size:  4 */
    uint32_t  ip6[4];         /*  Offset: 20, Size: 16 */
    uint8_t   mac_addr[6];    /*  Offset: 36, Size:  6 */
    uint8_t   name[32];       /*  Offset: 42, Size: 32 */
    uint16_t  crc16;          /*  Offset: 74, Size:  2 */
} __attribute__((packed));    /*  Totoal size: 76bytes */

void locator_init(void);
#endif /* end of _LOCATOR_H_ */
