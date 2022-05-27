#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

#include "lwip/opt.h"
#include "lwip/netifapi.h"
#include "lwip/tcpip.h"
#include "netif/ethernetif.h"
#include "lwip/apps/httpd.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "NuMicro.h"

#include "netif/m480_eth.h"

#include "udp_service.h"
#include "settings.h"
#include "utils.h"

extern void httpd_ext_init(void);

struct netif netif;

static unsigned int getLinkState(void)
{
  return (mdio_read(CONFIG_PHY_ADDR, MII_BMSR) & BMSR_LSTATUS) ? 1 : 0;
}

void vNetworkTask( void * pvParameters )
{
    ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gw;

    IP4_ADDR(&gw,
        getIPField(cfg.gateway, 1),
        getIPField(cfg.gateway, 2),
        getIPField(cfg.gateway, 3),
        getIPField(cfg.gateway, 4));

    IP4_ADDR(&ipaddr,
        getIPField(cfg.ip, 1),
        getIPField(cfg.ip, 2),
        getIPField(cfg.ip, 3),
        getIPField(cfg.ip, 4));

    IP4_ADDR(&netmask,
        getIPField(cfg.netmask, 1),
        getIPField(cfg.netmask, 2),
        getIPField(cfg.netmask, 3),
        getIPField(cfg.netmask, 4));

    tcpip_init(NULL, NULL);
    httpd_ext_init();
    httpd_init();
    udpService_init();

    // initial MAC address of NIC
    memcpy(netif.hwaddr, cfg.macaddr, 6);

    if (NULL == netif_add(&netif, &ipaddr, &netmask, &gw, NULL, ethernetif_init, tcpip_input)) {
      printf("Failed! Init NIC\n");
    }
    else {
      netif_set_hostname(&netif, cfg.hostname);
      netif_set_default(&netif);
      netif_set_up(&netif);
      netif_set_link_down(&netif);
    }
    NVIC_SetPriority(EMAC_TX_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1);
    NVIC_EnableIRQ(EMAC_TX_IRQn);
    NVIC_SetPriority(EMAC_RX_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1);
    NVIC_EnableIRQ(EMAC_RX_IRQn);

    for(;;) {
      if (getLinkState()) {
        if (!netif_is_link_up(&netif)) {
          reset_phy(); // need negotiation link speed again
          netif_set_link_up(&netif);
        }
      } else {
        if (netif_is_link_up(&netif)) {
          netif_set_link_down(&netif);
        }
      }
      vTaskDelay(500);
    }
}
