#ifndef _MAIN_H_
#define _MAIN_H_

#include "settings.h"
#include "serial.h"

void initDefaultCFG(void); // defined in src\webserver.c
void initNetwork(void); // defined in src\webserver.c
void ssi_ex_init(void);
void vNetworkTask(void * pvParameters); // defined in src\webserver.c

#endif /* end of #ifndef _MANIN_H_ */
