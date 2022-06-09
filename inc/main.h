#ifndef _MAIN_H_
#define _MAIN_H_

#ifdef __cplusplus
extern "C" {
#endif

enum {
    PORT0_IDX = 0,
    PORT1_IDX,
    MAX_PORTS
};

#include "settings.h"
#include "serial.h"

void initDefaultCFG(void); // defined in src\webserver.c
void initNetwork(void); // defined in src\webserver.c
void vNetworkTask(void * pvParameters); // defined in src\webserver.c

#ifdef __cplusplus
}
#endif

#endif /* end of #ifndef _MANIN_H_ */
