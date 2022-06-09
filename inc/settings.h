#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#ifdef __cplusplus
extern "C" {
#endif

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

// due to Flash erase, value preset '1'
enum {
    CFG_DATABLOCK_INUSE  = 0,
    CFG_DATABLOCK_UNUSE  = 1
};

enum {
    CFG_MODE_RAW = 0,
    CFG_MODE_TELNET = 1,
    CFG_MODE_VIRTUAL_COMM = 2,
};

enum {
    CFG_PROTOCOL_RAW = 0,
    CFG_PROTOCOL_485 = 1
};

enum {
    DEFAULT_TCP_DATA_PORT_BASE = 5000,
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
    uint32_t mode : 2;
    uint32_t protocol : 1;
    uint8_t  xon;
    uint8_t  xoff;
};

struct Configuration {
    uint32_t ip;
    uint32_t netmask;
    uint32_t gateway;
    struct   PortSettings port[MAX_PORTS];
    uint32_t static_ip : 1;
    uint32_t inUse: 1;
    uint32_t num : 4;
    uint8_t  macaddr[6];
    char     hostname[32];
    uint8_t  reserv[155]; /* keep the struct size always equal 256bytes */
    uint16_t crc;
};

#define SIZE_CONFIGURATION  (256)
#define MYASSERT(X, msg, ...)   \
    do { \
        if (!(X)) { \
          printf("FATAL@Line %d in %s\n" msg, __LINE__, __FILE__ __VA_OPT__(,) __VA_ARGS__); \
          for(;;); \
        } \
    } while(0)


void initCfgStruct(struct Configuration *pCfg);
extern struct Configuration cfg;

#ifdef __cplusplus
}
#endif

#endif /* end of #ifndef _SETTINGS_H_ */
