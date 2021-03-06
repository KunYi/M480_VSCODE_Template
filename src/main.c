
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

#include "NuMicro.h"
#include "lwip/apps/httpd.h"
#include "main.h"

// https://github.com/OpenNuvoton/ISPTool/blob/338ad2d8d6b9758a1ed7cd19e3101a316aedce3c/NuvoISP/DataBase/FlashInfo.cpp#L545

#define  DeviceID_M487JIDAE  (0x00d48750) // LQFP144, Flash:512KB, RAM:160KB
#define  DeviceID_M487KMCAN  (0x00D4874E) // LQFP128, Flash:2.5MB, RAM:128KB

#define  ENABLE_PDMA         (0)          // disable, future for highspeed baudrate
#define  DMA_BUFFER_LENGTH   (1024)
#define  PDMA_TIME           (0x5555)

#if ENABLE_PDMA
static uint8_t PortTxBuffer[MAX_PORTS][DMA_BUFFER_LENGTH];
static uint8_t PortRxBuffer[MAX_PORTS][DMA_BUFFER_LENGTH];
#endif

static void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Set XT1_OUT(PF.2) and XT1_IN(PF.3) to input mode */
    PF->MODE &= ~(GPIO_MODE_MODE2_Msk | GPIO_MODE_MODE3_Msk);

    /* Enable External XTAL (4~24 MHz) */
    CLK_EnableXtalRC(CLK_PWRCTL_HXTEN_Msk);

    /* Waiting for 12MHz clock ready */
    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);

    /* Set core clock as PLL_CLOCK from PLL */
    CLK_SetCoreClock(FREQ_192MHZ);
    /* Set PCLK0/PCLK1 to HCLK/2 */
    CLK->PCLKDIV = (CLK_PCLKDIV_APB0DIV_DIV2 | CLK_PCLKDIV_APB1DIV_DIV2);

    /* Enable UART clock */
    CLK_EnableModuleClock(UART0_MODULE);

    /* Port0/Port1 map to UART1/UART5 */
    CLK_EnableModuleClock(UART1_MODULE);
    CLK_EnableModuleClock(UART5_MODULE);

    /* Select UART clock source from HXT */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HXT, CLK_CLKDIV0_UART0(1));

    CLK_SetModuleClock(UART1_MODULE, CLK_CLKSEL1_UART1SEL_HXT, CLK_CLKDIV0_UART1(1));
    CLK_SetModuleClock(UART5_MODULE, CLK_CLKSEL3_UART5SEL_HXT, CLK_CLKDIV4_UART5(1));

    /* Enable EMAC clock */
    CLK_EnableModuleClock(EMAC_MODULE);

    // Configure MDC clock rate to HCLK / (127 + 1) = 1.5 MHz if system is running at 192 MHz
    CLK_SetModuleClock(EMAC_MODULE, 0, CLK_CLKDIV3_EMAC(127));


    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
    SystemCoreClockUpdate();

    /* Set GPB multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFPH &= ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk);
    SYS->GPB_MFPH |= (SYS_GPB_MFPH_PB12MFP_UART0_RXD | SYS_GPB_MFPH_PB13MFP_UART0_TXD);

    /* Set GPB multi-function pins for UART1 RXD, TXD, CTS and RTS */
    SYS->GPH_MFPH &= ~(SYS_GPH_MFPH_PH9MFP_Msk | SYS_GPH_MFPH_PH8MFP_Msk);
    SYS->GPH_MFPH |=  (SYS_GPH_MFPH_PH9MFP_UART1_RXD | SYS_GPH_MFPH_PH8MFP_UART1_TXD);
    SYS->GPB_MFPH &= ~(SYS_GPB_MFPH_PB9MFP_Msk | SYS_GPB_MFPH_PB8MFP_Msk);
    SYS->GPB_MFPH |= (SYS_GPB_MFPH_PB9MFP_UART1_nCTS | SYS_GPB_MFPH_PB8MFP_UART1_nRTS);

    /* Set GPB multi-function pins for UART5 RXD, TXD, CTS and RTS */
    SYS->GPB_MFPL &= ~(SYS_GPB_MFPL_PB2MFP_Msk | SYS_GPB_MFPL_PB3MFP_Msk |
                        SYS_GPB_MFPL_PB4MFP_Msk | SYS_GPB_MFPL_PB5MFP_Msk);
    SYS->GPB_MFPL |= ( SYS_GPB_MFPL_PB2MFP_UART5_nCTS |  SYS_GPB_MFPL_PB3MFP_UART5_nRTS |
                        SYS_GPB_MFPL_PB4MFP_UART5_RXD | SYS_GPB_MFPL_PB5MFP_UART5_TXD);

    /* set GPIOH for LED */
    PH->MODE = (PH->MODE & ~(GPIO_MODE_MODE0_Msk | GPIO_MODE_MODE1_Msk | GPIO_MODE_MODE2_Msk)) |
               (GPIO_MODE_OUTPUT << GPIO_MODE_MODE0_Pos) |
               (GPIO_MODE_OUTPUT << GPIO_MODE_MODE1_Pos) |
               (GPIO_MODE_OUTPUT << GPIO_MODE_MODE2_Pos);  // Set to output mode
    PH->DOUT = 3;

    // Configure RMII pins
    SYS->GPA_MFPL |= SYS_GPA_MFPL_PA6MFP_EMAC_RMII_RXERR | SYS_GPA_MFPL_PA7MFP_EMAC_RMII_CRSDV;
    SYS->GPC_MFPL |= SYS_GPC_MFPL_PC6MFP_EMAC_RMII_RXD1 | SYS_GPC_MFPL_PC7MFP_EMAC_RMII_RXD0;
    SYS->GPC_MFPH |= SYS_GPC_MFPH_PC8MFP_EMAC_RMII_REFCLK;
    SYS->GPE_MFPH |= SYS_GPE_MFPH_PE8MFP_EMAC_RMII_MDC |
                     SYS_GPE_MFPH_PE9MFP_EMAC_RMII_MDIO |
                     SYS_GPE_MFPH_PE10MFP_EMAC_RMII_TXD0 |
                     SYS_GPE_MFPH_PE11MFP_EMAC_RMII_TXD1 |
                     SYS_GPE_MFPH_PE12MFP_EMAC_RMII_TXEN;

    // Enable high slew rate on all RMII TX output pins
    PE->SLEWCTL = (GPIO_SLEWCTL_HIGH << GPIO_SLEWCTL_HSREN10_Pos) |
                  (GPIO_SLEWCTL_HIGH << GPIO_SLEWCTL_HSREN11_Pos) |
                  (GPIO_SLEWCTL_HIGH << GPIO_SLEWCTL_HSREN12_Pos);

    /* Lock protected registers */
    SYS_LockReg();
}

#if ENABLE_PDMA
void PDMA_Init(void)
{
    /* Open PDMA Channel */
    PDMA_Open(PDMA,1 << 0 | (1 << 1)); // Channel 0, 1 for UART1/UART5 RX, only ch0 and ch1 support PDMA timeout
    PDMA_Open(PDMA,2 << 1 | (3 << 1)); // Channel 2, 3 for UART1/UART5 TX
    // Select basic mode
    PDMA_SetTransferMode(PDMA,0, PDMA_UART1_RX, 0, 0);
    PDMA_SetTransferMode(PDMA,2, PDMA_UART1_TX, 0, 0);
    PDMA_SetTransferMode(PDMA,1, PDMA_UART5_RX, 0, 0);
    PDMA_SetTransferMode(PDMA,3, PDMA_UART5_TX, 0, 0);
    // Set data width and transfer count
    PDMA_SetTransferCnt(PDMA,0, PDMA_WIDTH_8, DMA_BUFFER_LENGTH);
    PDMA_SetTransferCnt(PDMA,2, PDMA_WIDTH_8, DMA_BUFFER_LENGTH);
    PDMA_SetTransferCnt(PDMA,1, PDMA_WIDTH_8, DMA_BUFFER_LENGTH);
    PDMA_SetTransferCnt(PDMA,3, PDMA_WIDTH_8, DMA_BUFFER_LENGTH);
    //Set PDMA Transfer Address
    PDMA_SetTransferAddr(PDMA,0, UART1_BASE, PDMA_SAR_FIX, ((uint32_t) (&PortRxBuffer[PORT0_IDX][0])), PDMA_DAR_INC);
    PDMA_SetTransferAddr(PDMA,2, ((uint32_t) (&PortTxBuffer[PORT0_IDX][0])), PDMA_SAR_INC, UART1_BASE, PDMA_DAR_FIX);
    PDMA_SetTransferAddr(PDMA,1, UART5_BASE, PDMA_SAR_FIX, ((uint32_t) (&PortRxBuffer[PORT1_IDX][0])), PDMA_DAR_INC);
    PDMA_SetTransferAddr(PDMA,3, ((uint32_t) (&PortTxBuffer[PORT1_IDX][0])), PDMA_SAR_INC, UART5_BASE, PDMA_DAR_FIX);
    //Select Single Request
    PDMA_SetBurstType(PDMA,0, PDMA_REQ_SINGLE, 0);
    PDMA_SetBurstType(PDMA,2, PDMA_REQ_SINGLE, 0);
    PDMA_SetBurstType(PDMA,1, PDMA_REQ_SINGLE, 0);
    PDMA_SetBurstType(PDMA,3, PDMA_REQ_SINGLE, 0);
    PDMA_EnableInt(PDMA,0, 0);
    PDMA_EnableInt(PDMA,2, 0);
    PDMA_EnableInt(PDMA,1, 0);
    PDMA_EnableInt(PDMA,3, 0);
    NVIC_EnableIRQ(PDMA_IRQn);
}

/**
 * @brief       DMA IRQ
 *
 * @param       None
 *
 * @return      None
 *
 * @details     The DMA default IRQ, declared in startup_M480.s.
 */
void PDMA_IRQHandler(void)
{
    uint32_t status = PDMA_GET_INT_STATUS(PDMA);

    if (status & 0x1)   /* abort */
    {
        printf("target abort interrupt !!\n");
        if (PDMA_GET_ABORT_STS(PDMA) & 0x4)
        PDMA_CLR_ABORT_FLAG(PDMA,PDMA_GET_ABORT_STS(PDMA));
    }
    else if (status & 0x2)     /* done */
    {
        if ( (PDMA_GET_TD_STS(PDMA) & (1 << 0)) && (PDMA_GET_TD_STS(PDMA) & (1 << 1)) )
        {

            PDMA_CLR_TD_FLAG(PDMA,PDMA_GET_TD_STS(PDMA));
        }
    }
    else if (status & 0x300)     /* channel 2 timeout */
    {
        printf("timeout interrupt !!\n");

        PDMA_SetTimeOut(PDMA,0, 0, 0);
        PDMA_CLR_TMOUT_FLAG(PDMA,0);
        PDMA_SetTimeOut(PDMA,0, 1, PDMA_TIME);

        PDMA_SetTimeOut(PDMA,1, 0, 0);
        PDMA_CLR_TMOUT_FLAG(PDMA,1);
        PDMA_SetTimeOut(PDMA,1, 1, PDMA_TIME);
    }
    else
        printf("unknown interrupt !!\n");
}
#endif

static void checkDeviceIsM487(void)
{
    uint8_t flag = 0;
    SYS_UnlockReg();                   /* Unlock register lock protect */
    FMC_Open();                        /* Enable FMC ISP function */
    const uint32_t pid = FMC_ReadPID();
    if (DeviceID_M487JIDAE != pid) {
        printf("FATAL:Not M487JIDAE: DeviceID:0x%08lX\n", pid);
        flag = 1;
    }
    FMC_Close();
    /* Lock protected registers */
    SYS_LockReg();
    while(flag);
}

/* Task to be created. */
void vLEDToggleTask( void * pvParameters )
{
    /* The parameter value is expected to be 1 as 1 is passed in the
    pvParameters value in the call to xTaskCreate() below.
    */
    configASSERT( ( ( uint32_t ) pvParameters ) == 1 );

    for( ;; )
    {
        /* Task code goes here. */
        static short i = 0;
        PH->DOUT = (i++ & 0x7); // total 3 LEDs
        vTaskDelay(200);
    }
}

static void prepareTasks(void)
{
    BaseType_t xReturned;
    TaskHandle_t xHandle = NULL;

    /* Create the task, storing the handle. */
    xReturned = xTaskCreate(
                    vLEDToggleTask,  /* Function that implements the task. */
                    "LEDToggle",     /* Text name for the task. */
                    100,             /* Stack size in words, not bytes. */
                    ( void * ) 1,    /* Parameter passed into the task. */
                    tskIDLE_PRIORITY,/* Priority at which the task is created. */
                    &xHandle );      /* Used to pass out the created task's handle. */

    if( xReturned == pdPASS ) {
        printf("Successful! Create LEDToggle task\n");
    }

    xReturned = xTaskCreate(
                    vNetworkTask,
                    "NetworkTask",
                    1024,
                    (void *) 0,
                    tskIDLE_PRIORITY,
                    &xHandle);
    if( xReturned == pdPASS ) {
        printf("Successful! Create WebServer task\n");
    }
}

static void initUART(UART_T *UART,const struct PortSettings *port)
{
    const uint32_t dlen = convPortDataLen(port->datalength);
    const uint32_t parity = convPortParity(port->parity);
    const uint32_t stopbits = convPortStopBits(port->stopbits);

    UART_Open(UART, port->baudrate);
    UART_SetLineConfig(UART, 0, dlen, parity, stopbits);
    UART_DisableFlowCtrl(UART);
    if (port->flowcontrol == FLOWCONTROL_RTSCTS)
        UART_EnableFlowCtrl(UART);
}

static void Ports_Init(void)
{
    initUART(UART1, &cfg.port[PORT0_IDX]);
    initUART(UART5, &cfg.port[PORT1_IDX]);
}

extern uint32_t readDevCfg(void);

int main(void) {
    /* Init System, IP clock and multi-function I/O */
    SYS_Init();
    UART_Open(UART0, 115200);
    checkDeviceIsM487();
    readDevCfg();
    Ports_Init();
#if ENABLE_PDMA
    PDMA_Init();
#endif
    /* Connect UART to PC, and open a terminal tool to receive following message */
    prepareTasks();
    printf("FreeRTOS is starting ...\n");

    /* Start the scheduler of FreeRTOS */
    vTaskStartScheduler();

    return 0;
}

/*-----------------------------------------------------------*/
void vApplicationStackOverflowHook( TaskHandle_t xTask, char * pcTaskName )
{
    ( void ) pcTaskName;
    ( void ) xTask;

    /* Run time stack overflow checking is performed if
    configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
    function is called if a stack overflow is detected. */
    taskDISABLE_INTERRUPTS();
    for( ;; );
}

/*-----------------------------------------------------------*/
void vApplicationTickHook( void )
{
    /* This function will be called by each tick interrupt if
    configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h.  User code can be
    added here, but the tick hook is called from an interrupt context, so
    code must not attempt to block, and only the interrupt safe FreeRTOS API
    functions can be used (those that end in FromISR()).  */

    {
        /* In this case the tick hook is used as part of the queue set test. */
        /* vQueueSetAccessQueueSetFromISR(); */
    }
}

/*-----------------------------------------------------------*/
void vApplicationIdleHook(void)
{

}

/*-----------------------------------------------------------*/
void vApplicationMallocFailedHook(void)
{
    taskDISABLE_INTERRUPTS();
    for( ;; );
}
