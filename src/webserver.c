
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

#include "lwip/netifapi.h"
#include "lwip/tcpip.h"
#include "netif/ethernetif.h"

#include "NuMicro.h"

unsigned char my_mac_addr[6] = {0x00, 0x00, 0x00, 0x55, 0x66, 0x77};
struct netif netif;

void initNetwork(void)
{
    ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gw;

    IP4_ADDR(&gw, 192, 168, 10, 1);
    IP4_ADDR(&ipaddr, 192, 168, 10, 120);
    IP4_ADDR(&netmask, 255, 255, 255,0);

    tcpip_init(NULL, NULL);

    netif_add(&netif, &ipaddr, &netmask, &gw, NULL, ethernetif_init, tcpip_input);

    netif_set_default(&netif);
    netif_set_up(&netif);

    NVIC_SetPriority(EMAC_TX_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1);
    NVIC_EnableIRQ(EMAC_TX_IRQn);
    NVIC_SetPriority(EMAC_RX_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1);
    NVIC_EnableIRQ(EMAC_RX_IRQn);
}
