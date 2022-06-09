#ifndef _MAIN_H_
#define _MAIN_H_

#include "settings.h"
#include "serial.h"

enum {
    PORT0_IDX = 0,
    PORT1_IDX,
    MAX_PORTS
};

void initDefaultCFG(void); // defined in src\webserver.c
void initNetwork(void); // defined in src\webserver.c
void ssi_ex_init(void);
void vNetworkTask(void * pvParameters); // defined in src\webserver.c

#endif /* end of #ifndef _MANIN_H_ */
