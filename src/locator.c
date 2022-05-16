#include "lwip/opt.h"
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/udp.h"

#define UDP_PORT  (23)
static struct udp_pcb *udp_raw_pcb;


static void
udp_raw_recv(void *arg, struct udp_pcb *upcb, struct pbuf *p,
                 const ip_addr_t *addr, u16_t port)
{
  LWIP_UNUSED_ARG(arg);
  if (p != NULL) {
    /* send received packet back to sender */
    udp_sendto(upcb, p, addr, port);
    /* free the pbuf */
    pbuf_free(p);
  }
}

void locator_init(void)
{
  udp_raw_pcb = udp_new_ip_type(IPADDR_TYPE_ANY);
  if (udp_raw_pcb != NULL) {
    err_t err;

    err = udp_bind(udp_raw_pcb, IP_ANY_TYPE, UDP_PORT);
    if (err == ERR_OK) {
      udp_recv(udp_raw_pcb, udp_raw_recv, NULL);
    } else {
      /* abort? output diagnostic? */
    }
  } else {
    /* abort? output diagnostic? */
  }
}
