#include <stdint.h>
#include "lwip/opt.h"
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"

#include "NuMicro.h"
#include "main.h"
#include "utils.h"

static const uint8_t       portIdx[MAX_PORTS] = { 0, 1 };
static struct tcp_pcb *tcp_raw_pcb[MAX_PORTS];

enum tcp_raw_states
{
  ES_NONE = 0,
  ES_ACCEPTED,
  ES_RECEIVED,
  ES_CLOSING
};

struct tcp_raw_state
{
  uint8_t port;
  uint8_t state;
  uint8_t retries;
  struct tcp_pcb *pcb;
  /* pbuf (chain) to recycle */
  struct pbuf *p;
};

static void
tcp_raw_free(struct tcp_raw_state *es)
{
  if (es != NULL) {
    if (es->p) {
      /* free the buffer chain if present */
      pbuf_free(es->p);
    }

    mem_free(es);
  }
}


static void
tcp_raw_close(struct tcp_pcb *tpcb, struct tcp_raw_state *es)
{
  tcp_arg(tpcb, NULL);
  tcp_sent(tpcb, NULL);
  tcp_recv(tpcb, NULL);
  tcp_err(tpcb, NULL);
  tcp_poll(tpcb, NULL, 0);

  tcp_raw_free(es);

  tcp_close(tpcb);
}

static void
tcp_raw_send(struct tcp_pcb *tpcb, struct tcp_raw_state *es)
{
  struct pbuf *ptr;
  err_t wr_err = ERR_OK;

  while ((wr_err == ERR_OK) &&
         (es->p != NULL) &&
         (es->p->len <= tcp_sndbuf(tpcb))) {
    ptr = es->p;

    /* enqueue data for transmission */
    wr_err = tcp_write(tpcb, ptr->payload, ptr->len, 1);
    if (wr_err == ERR_OK) {
      u16_t plen;

      plen = ptr->len;
      /* continue with next pbuf in chain (if any) */
      es->p = ptr->next;
      if(es->p != NULL) {
        /* new reference! */
        pbuf_ref(es->p);
      }
      /* chop first pbuf from chain */
      pbuf_free(ptr);
      /* we can read more data now */
      tcp_recved(tpcb, plen);
    } else if(wr_err == ERR_MEM) {
      /* we are low on memory, try later / harder, defer to poll */
      es->p = ptr;
    } else {
      /* other problem ?? */
    }
  }
}

static void
tcp_raw_error(void *arg, err_t err)
{
  struct tcp_raw_state *es;

  LWIP_UNUSED_ARG(err);

  es = (struct tcp_raw_state *)arg;

  tcp_raw_free(es);
}

static err_t
tcp_raw_poll(void *arg, struct tcp_pcb *tpcb)
{
  err_t ret_err;
  struct tcp_raw_state *es;

  es = (struct tcp_raw_state *)arg;
  if (es != NULL) {
    if (es->p != NULL) {
      /* there is a remaining pbuf (chain)  */
      tcp_raw_send(tpcb, es);
    } else {
      /* no remaining pbuf (chain)  */
      if(es->state == ES_CLOSING) {
        tcp_raw_close(tpcb, es);
      }
    }
    ret_err = ERR_OK;
  } else {
    /* nothing to be done */
    tcp_abort(tpcb);
    ret_err = ERR_ABRT;
  }
  return ret_err;
}

static err_t
tcp_raw_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
  struct tcp_raw_state *es;

  LWIP_UNUSED_ARG(len);

  es = (struct tcp_raw_state *)arg;
  es->retries = 0;

  if(es->p != NULL) {
    /* still got pbufs to send */
    tcp_sent(tpcb, tcp_raw_sent);
    tcp_raw_send(tpcb, es);
  } else {
    /* no more pbufs to send */
    if(es->state == ES_CLOSING) {
      tcp_raw_close(tpcb, es);
    }
  }
  return ERR_OK;
}

static err_t
tcp_raw_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
  struct tcp_raw_state *es;
  err_t ret_err;

  LWIP_ASSERT("arg != NULL",arg != NULL);
  es = (struct tcp_raw_state *)arg;
  if (p == NULL) {
    /* remote host closed connection */
    es->state = ES_CLOSING;
    if(es->p == NULL) {
      /* we're done sending, close it */
      tcp_raw_close(tpcb, es);
    } else {
      /* we're not done yet */
      tcp_raw_send(tpcb, es);
    }
    ret_err = ERR_OK;
  } else if(err != ERR_OK) {
    /* cleanup, for unknown reason */
    if (p != NULL) {
      pbuf_free(p);
    }
    ret_err = err;
  }
  else if(es->state == ES_ACCEPTED) {
    /* first data chunk in p->payload */
    es->state = ES_RECEIVED;
    /* store reference to incoming pbuf (chain) */
    es->p = p;
    tcp_raw_send(tpcb, es);
    ret_err = ERR_OK;
  } else if (es->state == ES_RECEIVED) {
    /* read some more data */
    if(es->p == NULL) {
      es->p = p;
      tcp_raw_send(tpcb, es);
    } else {
      struct pbuf *ptr;

      /* chain pbufs to the end of what we recv'ed previously  */
      ptr = es->p;
      pbuf_cat(ptr,p);
    }
    ret_err = ERR_OK;
  } else {
    /* unkown es->state, trash data  */
    tcp_recved(tpcb, p->tot_len);
    pbuf_free(p);
    ret_err = ERR_OK;
  }
  return ret_err;
}

static err_t
tcp_raw_accept(void *arg, struct tcp_pcb *newpcb, err_t err)
{
  err_t ret_err = ERR_OK;
  struct tcp_raw_state *es;


  if ((err != ERR_OK) || (newpcb == NULL)) {
    return ERR_VAL;
  }

  /* Unless this pcb should have NORMAL priority, set its priority now.
     When running out of pcbs, low priority pcbs can be aborted to create
     new pcbs of higher priority. */
  tcp_setprio(newpcb, TCP_PRIO_MIN);

  es = (struct tcp_raw_state *)mem_malloc(sizeof(struct tcp_raw_state));
  if (es != NULL) {
    es->port = *((uint8_t*)arg);
    es->state = ES_ACCEPTED;
    es->pcb = newpcb;
    es->retries = 0;
    es->p = NULL;
    /* pass newly allocated es to our callbacks */
    tcp_arg(newpcb, es);
    tcp_recv(newpcb, tcp_raw_recv);
    tcp_err(newpcb, tcp_raw_error);
    tcp_poll(newpcb, tcp_raw_poll, 0);
    tcp_sent(newpcb, tcp_raw_sent);
    ret_err = ERR_OK;
  } else {
    ret_err = ERR_MEM;
  }

  return ret_err;
}

static void prepareTcp4Port(const uint32_t Idx, const struct PortSettings *port, struct tcp_pcb *pcb)
{
  pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);

  if (pcb != NULL) {
    err_t err;

    err = tcp_bind(pcb, IP_ANY_TYPE, port->localport);
    if (err == ERR_OK) {
      pcb = tcp_listen(pcb);
      pcb->callback_arg = (uint8_t*)((PORT0_IDX == Idx) ? &portIdx[PORT0_IDX] : &portIdx[PORT1_IDX]);
      tcp_accept(pcb, tcp_raw_accept);
    } else {
      /* abort? output diagnostic? */
    }
  } else {
    /* abort? output diagnostic? */
  }
}

void tcpServer_init(void)
{
  prepareTcp4Port(PORT0_IDX, &cfg.port[PORT0_IDX], tcp_raw_pcb[PORT0_IDX]);
  prepareTcp4Port(PORT1_IDX, &cfg.port[PORT1_IDX], tcp_raw_pcb[PORT1_IDX]);
}
