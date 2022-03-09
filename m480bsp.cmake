

	add_library(m480bsp INTERFACE)
	set(M480BSP_PATH ${CMAKE_CURRENT_LIST_DIR}/M480BSP/Library)
	set(M480BSP_STDDRIVE_PATH ${M480BSP_PATH}/StdDriver)
	set(M480BSP_CMSIS_PATH ${M480BSP_PATH}/CMSIS)


	target_sources(m480bsp INTERFACE
		${M480BSP_PATH}/Device/Nuvoton/M480/Source/system_M480.c
		${M480BSP_PATH}/Device/Nuvoton/M480/Source/GCC/startup_M480.S
		${M480BSP_STDDRIVE_PATH}/src/acmp.c
		${M480BSP_STDDRIVE_PATH}/src/bpwm.c
		${M480BSP_STDDRIVE_PATH}/src/can.c
		${M480BSP_STDDRIVE_PATH}/src/ccap.c
		${M480BSP_STDDRIVE_PATH}/src/clk.c
		${M480BSP_STDDRIVE_PATH}/src/crc.c
		${M480BSP_STDDRIVE_PATH}/src/crypto.c
		${M480BSP_STDDRIVE_PATH}/src/dac.c
		${M480BSP_STDDRIVE_PATH}/src/eadc.c
		${M480BSP_STDDRIVE_PATH}/src/ebi.c
		${M480BSP_STDDRIVE_PATH}/src/ecap.c
		${M480BSP_STDDRIVE_PATH}/src/emac.c
		${M480BSP_STDDRIVE_PATH}/src/epwm.c
		${M480BSP_STDDRIVE_PATH}/src/fmc.c	
		${M480BSP_STDDRIVE_PATH}/src/gpio.c
		${M480BSP_STDDRIVE_PATH}/src/hsusbd.c
		${M480BSP_STDDRIVE_PATH}/src/i2c.c
		${M480BSP_STDDRIVE_PATH}/src/i2s.c
		${M480BSP_STDDRIVE_PATH}/src/pdma.c
		${M480BSP_STDDRIVE_PATH}/src/qei.c
		${M480BSP_STDDRIVE_PATH}/src/qspi.c
		${M480BSP_STDDRIVE_PATH}/src/retarget.c
		${M480BSP_STDDRIVE_PATH}/src/rtc.c
		${M480BSP_STDDRIVE_PATH}/src/sc.c
		${M480BSP_STDDRIVE_PATH}/src/scuart.c
		${M480BSP_STDDRIVE_PATH}/src/sdh.c
		${M480BSP_STDDRIVE_PATH}/src/spi.c
		${M480BSP_STDDRIVE_PATH}/src/spim.c
		${M480BSP_STDDRIVE_PATH}/src/sys.c
		${M480BSP_STDDRIVE_PATH}/src/timer.c
		${M480BSP_STDDRIVE_PATH}/src/timer_pwm.c
		${M480BSP_STDDRIVE_PATH}/src/trng.c
		${M480BSP_STDDRIVE_PATH}/src/uart.c
		${M480BSP_STDDRIVE_PATH}/src/usbd.c
		${M480BSP_STDDRIVE_PATH}/src/usci_i2c.c
		${M480BSP_STDDRIVE_PATH}/src/usci_spi.c
		${M480BSP_STDDRIVE_PATH}/src/usci_uart.c
		${M480BSP_STDDRIVE_PATH}/src/wdt.c
		${M480BSP_STDDRIVE_PATH}/src/wwdt.c
	)

	target_include_directories(m480bsp INTERFACE
		${M480BSP_PATH}/Device/Nuvoton/M480/Include
		${M480BSP_STDDRIVE_PATH}/inc
		${M480BSP_CMSIS_PATH}/Include
	)
