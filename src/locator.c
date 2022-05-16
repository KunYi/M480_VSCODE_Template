#include "lwip/opt.h"
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/udp.h"

#include "locator.h"

static struct udp_pcb *udp_raw_pcb;
static struct UDP_DISCOVERY_RESPONSE reponse;
static const struct UDP_DISCOVERY_COMMAND discovery_cmd =
  { .tag[0] = TAG0, .tag[1] = TAG1,
    .length = sizeof(struct UDP_DISCOVERY_COMMAND),
    .pktver = 0, .reserv = 0, .crc16 = 0xffff };

static void
udp_raw_recv(void *arg, struct udp_pcb *upcb, struct pbuf *p,
                 const ip_addr_t *addr, u16_t port)
{
  LWIP_UNUSED_ARG(arg);
  if (p != NULL) {
    if (p->len == sizeof(struct UDP_DISCOVERY_COMMAND) &&
        0 == memcmp(&discovery_cmd, p->payload, sizeof(struct UDP_DISCOVERY_COMMAND)))
    {
      /* free the pbuf */
      pbuf_free(p);

      /* send received packet back to sender */
      p = pbuf_alloc(PBUF_TRANSPORT, sizeof(reponse), PBUF_RAM);
      if (p == NULL) {
        return;
      }
      memcpy(p->payload, &reponse, sizeof(reponse));
      udp_sendto(upcb, p, addr, port);
    }
  }

  if(p != NULL) {
    /* free the pbuf */
    pbuf_free(p);
  }
}

void locator_init(void)
{
  udp_raw_pcb = udp_new_ip_type(IPADDR_TYPE_ANY);
  if (udp_raw_pcb != NULL) {
    err_t err;

    memset(&reponse, 0, sizeof(reponse));

    err = udp_bind(udp_raw_pcb, IP_ANY_TYPE, LOCATOR_PORT);
    if (err == ERR_OK) {
      udp_recv(udp_raw_pcb, udp_raw_recv, NULL);
    } else {
      /* abort? output diagnostic? */
    }
  } else {
    /* abort? output diagnostic? */
  }
}
