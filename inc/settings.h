#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#define SUPPORT_PORTS    (2)
enum {
    PARITY_NONE = 0,
    PARITY_EVEN,
    PARITY_ODD,
    PARITY_MARK,
    PARITY_SPACE,
};

enum {
    STOPBIT_1 = 0,
    STOPBIT_1D5,
    STOPBIT_2,
};

enum {
    FLOWCONTROL_NONE = 0,
    FLOWCONTROL_RTSCTS,
    FLOWCONTROL_DSRDTR,
    FLOWCONTROL_XONXOFF,
};

struct PortSettings {
    uint32_t baudrate;
    uint8_t  datalength;
    uint8_t  parity;
    uint8_t  stopbits;
    uint8_t  flowcontrol;
    uint16_t localport;
    uint16_t remoteport;
    uint32_t timeout;
    uint32_t ip;
    uint32_t mode : 1;
    uint32_t protocol : 1;
    uint8_t  xon;
    uint8_t  xoff;
} __attribute__((packed));


struct Configuration {
    uint32_t ip;
    uint32_t netmask;
    uint32_t gateway;
    struct   PortSettings port[SUPPORT_PORTS];
    uint32_t static_ip : 1;
    uint8_t  macaddr[6];
    char     hostname[32];
} __attribute__((packed));

#endif /* end of #ifndef _SETTINGS_H_ */
