#!/bin/bash

CFLAGS="-O3 -DMAKEFS_SUPPORT_DEFLATE"
BSP_DIR=../../../../../..
LWIP_DIR=${BSP_DIR}/ThirdParty/LwIP
FREERTOS_DIR=${BSP_DIR}/ThirdParty/FreeRTOS
gcc makefsdata.c  -I${LWIP_DIR}/src/include -I ${BSP_DIR}/inc \
	-I${FREERTOS_DIR}/include -I${FREERTOS_DIR}/portable/GCC/ARM_CM4_MPU \
	-I ${BSP_DIR}/port/lwip/include ${CFLAGS} -o makefsdata.exe 