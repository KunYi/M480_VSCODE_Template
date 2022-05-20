
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

#include "settings.h"
#include "utils.h"

#include <stdio.h>
#include <string.h>

void httpd_ext_init(void);
static void cgi_ex_init(void);
static void ssi_ex_init(void);
extern struct Configuration cfg;

/*
 * -------------------------------------------------------------------------------------------------------
 *  CGI Support Part
 * -------------------------------------------------------------------------------------------------------
 */
#if !LWIP_HTTPD_CGI
#error LWIP_HTTPD_EXAMPLE_CGI_SIMPLE needs LWIP_HTTPD_CGI
#endif

static const char *cgi_handler_config(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);
static const char *cgi_handler_ip(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);
static const char *cgi_handler_misc(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);
static const char *cgi_handler_defaults(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);

static const tCGI cgi_handlers[] = {
  {
    "/config.cgi",
    cgi_handler_config
  },
  {
    "/ip.cgi",
    cgi_handler_ip
  },
  {
    "/misc.cgi",
    cgi_handler_misc
  },
  {
    "/defaults.cgi",
    cgi_handler_defaults
  }
};

static void cgi_ex_init(void)
{
  http_set_cgi_handlers(cgi_handlers, LWIP_ARRAYSIZE(cgi_handlers));
}

/** This config CGI function can parse param/value pairs and return an url that
 * is sent as a response by httpd.
 *
 * This example function just checks that the input url has two key value
 * parameter pairs: "foo=bar" and "test=123"
 * If not, it returns 404
 */
static const char *
cgi_handler_config(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
  LWIP_ASSERT("check index", iIndex < LWIP_ARRAYSIZE(cgi_handlers));

  if (iNumParams == 2) {
    if (!strcmp(pcParam[0], "foo")) {
      if (!strcmp(pcValue[0], "bar")) {
        if (!strcmp(pcParam[1], "test")) {
          if (!strcmp(pcValue[1], "123")) {
            return "/index.html";
          }
        }
      }
    }
  }
  return "/404.html";
}

static const char *
cgi_handler_ip(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
  LWIP_ASSERT("check index", iIndex < LWIP_ARRAYSIZE(cgi_handlers));

  if (iNumParams == 2) {
    if (!strcmp(pcParam[0], "foo")) {
      if (!strcmp(pcValue[0], "bar")) {
        if (!strcmp(pcParam[1], "test")) {
          if (!strcmp(pcValue[1], "123")) {
            return "/index.html";
          }
        }
      }
    }
  }
  return "/404.html";
}

static const char *
cgi_handler_misc(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
  LWIP_ASSERT("check index", iIndex < LWIP_ARRAYSIZE(cgi_handlers));
  //printf("pcParam[0]:%s, pcValue[0]:%s\n", pcParam[0], pcValue[0]);
  if (iNumParams == 2) {
    if (!strcmp(pcParam[0], "foo")) {
      if (!strcmp(pcValue[0], "bar")) {
        if (!strcmp(pcParam[1], "test")) {
          if (!strcmp(pcValue[1], "123")) {
            return "/index.html";
          }
        }
      }
    }
  }
  return "/404.html";
}

static const char *
cgi_handler_defaults(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
  LWIP_ASSERT("check index", iIndex < LWIP_ARRAYSIZE(cgi_handlers));
  printf("pcParam[0]:%s, pcValue[0]:%s\n", pcParam[0], pcValue[0]);
  if (iNumParams == 2) {
    if (!strcmp(pcParam[0], "foo")) {
      if (!strcmp(pcValue[0], "bar")) {
        if (!strcmp(pcParam[1], "test")) {
          if (!strcmp(pcValue[1], "123")) {
            return "/index.html";
          }
        }
      }
    }
  }
  return "/404.html";
}


/*
 * -------------------------------------------------------------------------------------------------------
 *  End of CGI Support Part
 * -------------------------------------------------------------------------------------------------------
 */

/*
 * -------------------------------------------------------------------------------------------------------
 *  SSI Support Part
 * -------------------------------------------------------------------------------------------------------
 */
/** define LWIP_HTTPD_EXAMPLE_SSI_SIMPLE_CGI_INTEGRATION to 1 to show how to
 * integrate CGI into SSI (LWIP_HTTPD_CGI_SSI) */

#if !LWIP_HTTPD_CGI_SSI
#error LWIP_HTTPD_EXAMPLE_SSI_SIMPLE_CGI_INTEGRATION needs LWIP_HTTPD_CGI_SSI
#endif

#define MAX_CGI_LEN   16

enum  _SSI_TAGS_INDEX {
  SSI_IDX_IPADDR = 0,
  SSI_IDX_MACADDR,
  SSI_IDX_MODNAME,
  SSI_IDX_PORT0_BAUDRATE,
  SSI_IDX_PORT0_DATALENGTH,
  SSI_IDX_PORT0_PARITY,
  SSI_IDX_PORT0_STOPBITS,
  SSI_IDX_PORT0_FLOWCONTROL,
  SSI_IDX_PORT1_BAUDRATE,
  SSI_IDX_PORT1_DATALENGTH,
  SSI_IDX_PORT1_PARITY,
  SSI_IDX_PORT1_STOPBITS,
  SSI_IDX_PORT1_FLOWCONTROL,
  SSI_IDX_PORT0_TLP,
  SSI_IDX_PORT0_TRP,
  SSI_IDX_PORT0_TNM,
  SSI_IDX_PORT0_TPROT,
  SSI_IDX_PORT0_TIP,
  SSI_IDX_PORT0_TTO,
  SSI_IDX_PORT1_TLP,
  SSI_IDX_PORT1_TRP,
  SSI_IDX_PORT1_TNM,
  SSI_IDX_PORT1_TPROT,
  SSI_IDX_PORT1_TIP,
  SSI_IDX_PORT1_TTO,
  SSI_IDX_P0CFG_VARS,
  SSI_IDX_P1CFG_VARS,
  SSI_IDX_MISC_VARS,
  SSI_IDX_HOSTNAME_INP,
};

const char * ssi_example_tags[] = {
  "ipaddr",
  "macaddr",
  "modname",
  "p0br", /* baudrate */
  "p0dl", /* data length */
  "p0p",  /* parity */
  "p0sb", /* stop bits */
  "p0fc", /* flow control */
  "p1br", /* baudrate */
  "p1dl", /* data length */
  "p1p",  /* parity */
  "p1sb", /* stop bits */
  "p1fc", /* flow control */
  "p0tlp",  /* local telnet port */
  "p0trp",  /* remote telnet port */
  "p0tnm",  /* telmode */
  "p0prot", /* telnet protocol */
  "p0tip",  /* telnet server ip */
  "p0tt",   /* telnet timeout */
  "p1tlp",  /* local telnet port */
  "p1trp",  /* remote telnet port */
  "p1tnm",  /* telmode */
  "p1prot", /* telnet protocol */
  "p1tip",  /* telnet server ip */
  "p1tt",   /* telnet timeout */
  "p0cvars",
  "p1cvars",
  "misvars",
  "hstninp", /* hostname name input */
#if LWIP_HTTPD_EXAMPLE_SSI_SIMPLE_CGI_INTEGRATION
  ,"CgiParam"
#endif
};

const char script_start[] = "<script type='text/javascript'><!--\n";
const char script_end[] = "--></script>\n";

const char ser_vars_fmt[] = \
  "var br = %u;\n"   \
  "var dl = %u;\n"   \
  "var par = %u;\n"  \
  "var sb = %u;\n"   \
  "var fc = %u;\n";

const char telnet_vars_fmt[] = \
  "var tt = %u;\n"   \
  "var tlp = %u;\n"  \
  "var trp = %u;\n"  \
  "var tnm = %u;\n"  \
  "var tnp = %u;\n";

const char telnet_ip_vars_fmt[] = \
  "var tip1 = %d;\n" \
  "var tip2 = %d;\n" \
  "var tip3 = %d;\n" \
  "var tip4 = %d;\n";

const char static_ip_vars_fmt[] = \
  "var staticip = %d;\n"  \
  "var sip1 = %d;\n"      \
  "var sip2 = %d;\n"      \
  "var sip3 = %d;\n"      \
  "var sip4 = %d;\n";

const char netmask_vars_fmt[] = \
  "var mip1 = %d;\n"  \
  "var mip2 = %d;\n"  \
  "var mip3 = %d;\n"  \
  "var mip4 = %d;\n";

const char gateway_vars_fmt[] = \
  "var gip1 = %d;\n"  \
  "var gip2 = %d;\n"  \
  "var gip3 = %d;\n"  \
  "var gip4 = %d;\n";

const char hostname_inp_vars_fmt[] = \
  "<input maxlength=32 name=devicename value=\"%s\">";

u16_t ssi_example_ssi_handler(
#if LWIP_HTTPD_SSI_RAW
                             const char* ssi_tag_name,
#else /* LWIP_HTTPD_SSI_RAW */
                             int iIndex,
#endif /* LWIP_HTTPD_SSI_RAW */
                             char *pcInsert, int iInsertLen
#if LWIP_HTTPD_SSI_MULTIPART
                             , u16_t current_tag_part, u16_t *next_tag_part
#endif /* LWIP_HTTPD_SSI_MULTIPART */
#if defined(LWIP_HTTPD_FILE_STATE) && LWIP_HTTPD_FILE_STATE
                             , void *connection_state
#endif /* LWIP_HTTPD_FILE_STATE */
                             )
{
  size_t printed;
#if LWIP_HTTPD_SSI_RAW
  /* a real application could use if(!strcmp) blocks here, but we want to keep
     the differences between configurations small, so translate string to index here */
  int iIndex;
  for (iIndex = 0; iIndex < LWIP_ARRAYSIZE(ssi_example_tags); iIndex++) {
    if(!strcmp(ssi_tag_name, ssi_example_tags[iIndex])) {
      break;
    }
  }
#endif
#if defined(LWIP_HTTPD_FILE_STATE) && LWIP_HTTPD_FILE_STATE
  LWIP_UNUSED_ARG(connection_state);
#endif

  switch (iIndex) {
  case SSI_IDX_IPADDR:
    printed = snprintf(pcInsert, iInsertLen, "192.168.10.120");
    break;
  case SSI_IDX_MACADDR:
    {
      printed = snprintf(pcInsert, iInsertLen, "%02X:%02X:%02X:%02X:%02X:%02X",
          cfg.macaddr[0], cfg.macaddr[1], cfg.macaddr[2],
          cfg.macaddr[3], cfg.macaddr[4], cfg.macaddr[5]);
    }
    break;
  case SSI_IDX_MODNAME:
    {
       printed = snprintf(pcInsert, iInsertLen, "%s", "UxConn");
    }
    break;
  case SSI_IDX_PORT0_BAUDRATE:
  case SSI_IDX_PORT1_BAUDRATE:
    printed = snprintf(pcInsert, iInsertLen,  "%d", 115200);
    break;
  case SSI_IDX_PORT0_DATALENGTH:
  case SSI_IDX_PORT1_DATALENGTH:
    printed = snprintf(pcInsert, iInsertLen,  "%d", 8);
    break;
  case SSI_IDX_PORT0_PARITY:
  case SSI_IDX_PORT1_PARITY:
    printed = snprintf(pcInsert, iInsertLen, "%s", "None");
    break;
  case SSI_IDX_PORT0_STOPBITS:
  case SSI_IDX_PORT1_STOPBITS:
    printed = snprintf(pcInsert, iInsertLen, "%d", 1);
    break;
  case SSI_IDX_PORT0_FLOWCONTROL:
  case SSI_IDX_PORT1_FLOWCONTROL:
    printed = snprintf(pcInsert, iInsertLen, "%s", "None");
    break;
  case SSI_IDX_PORT0_TLP:
  case SSI_IDX_PORT1_TLP:
    printed = snprintf(pcInsert, iInsertLen, "%d", 23);
    break;
  case SSI_IDX_PORT0_TRP:
  case SSI_IDX_PORT1_TRP:
    printed = snprintf(pcInsert, iInsertLen, "%d", 23);
    break;
  case SSI_IDX_PORT0_TNM:
  case SSI_IDX_PORT1_TNM:
    printed = snprintf(pcInsert, iInsertLen, "%s", "Server");
    break;
  case SSI_IDX_PORT0_TPROT:
  case SSI_IDX_PORT1_TPROT:
    printed = snprintf(pcInsert, iInsertLen, "%s", "Telnet");
    break;
  case SSI_IDX_PORT0_TIP:
  case SSI_IDX_PORT1_TIP:
    printed = (iIndex == SSI_IDX_PORT0_TIP) ?
      snprintf(pcInsert, iInsertLen, "%s", "192.168.10.1") :
      snprintf(pcInsert, iInsertLen, "%s", "192.168.10.2") ;
    break;
  case SSI_IDX_PORT0_TTO:
  case SSI_IDX_PORT1_TTO:
    printed = snprintf(pcInsert, iInsertLen, "%d", 0);
    break;
  case SSI_IDX_P0CFG_VARS:
  case SSI_IDX_P1CFG_VARS:
    switch (current_tag_part) {
    case 0:
      printed = snprintf(pcInsert, iInsertLen, "%s", script_start);
      *next_tag_part = 1;
      break;
    case 1:
      {
        const struct PortSettings *port = (iIndex == SSI_IDX_P0CFG_VARS) ? &cfg.port[0] : &cfg.port[1];
        printed = snprintf(pcInsert, iInsertLen, ser_vars_fmt,
                  (unsigned)port->baudrate, (unsigned)port->datalength,
                  (unsigned)port->parity, (unsigned)port->stopbits, (unsigned)port->flowcontrol);
        *next_tag_part = 2;
      }
      break;
    case 2:
      {
        const struct PortSettings *port = (iIndex == SSI_IDX_P0CFG_VARS) ? &cfg.port[0] : &cfg.port[1];
        printed = snprintf(pcInsert, iInsertLen, telnet_vars_fmt,
                  (unsigned)port->timeout,
                  (unsigned)port->localport,
                  (unsigned)port->remoteport,
                  port->mode ? 1 : 0 , port->protocol ? 1 : 0);
        *next_tag_part = 3;
      }
      break;
    case 3:
      {
        const struct PortSettings *port = (iIndex == SSI_IDX_P0CFG_VARS) ? &cfg.port[0] : &cfg.port[1];
        printed = snprintf(pcInsert, iInsertLen, telnet_ip_vars_fmt,
                    getIPField(port->ip, 1), getIPField(port->ip, 2),
                    getIPField(port->ip, 3), getIPField(port->ip, 4));
        *next_tag_part = 4;
      }
      break;
    case 4:
      printed = snprintf(pcInsert, iInsertLen, "%s", "SetCfgFormDefaults();\n");
      *next_tag_part = 5;
      break;
    case 5:
      printed = snprintf(pcInsert, iInsertLen, "%s", script_end);
      // don't change *next_tag_part value for last part
      break;
    default:
      break;
    }

    break;
  case SSI_IDX_MISC_VARS:
    switch (current_tag_part) {
    case 0:
      printed = snprintf(pcInsert, iInsertLen, "%s", script_start);
      *next_tag_part = 1;
      break;
    case 1:
      printed = snprintf(pcInsert, iInsertLen, static_ip_vars_fmt,
        cfg.static_ip ? 1 : 0,
        getIPField(cfg.ip, 1), getIPField(cfg.ip, 2),
        getIPField(cfg.ip, 3), getIPField(cfg.ip, 4));
      *next_tag_part = 2;
      break;
    case 2:
      printed = snprintf(pcInsert, iInsertLen, netmask_vars_fmt,
        getIPField(cfg.netmask, 1), getIPField(cfg.netmask, 2),
        getIPField(cfg.netmask, 3), getIPField(cfg.netmask, 4));
      *next_tag_part = 3;
      break;
    case 3:
      printed = snprintf(pcInsert, iInsertLen, gateway_vars_fmt,
        getIPField(cfg.gateway, 1), getIPField(cfg.gateway, 2),
        getIPField(cfg.gateway, 3), getIPField(cfg.gateway, 4));
      *next_tag_part = 4;
      break;
    case 4:
      printed = snprintf(pcInsert, iInsertLen, "%s", "SetMiscFormDefaults();\n");
      *next_tag_part = 5;
      break;
    case 5:
      printed = snprintf(pcInsert, iInsertLen, "%s", script_end);
      // don't change *next_tag_part value for last part
      break;
    default:
      break;
    }
    break;
  case SSI_IDX_HOSTNAME_INP:
    printed = snprintf(pcInsert, iInsertLen, hostname_inp_vars_fmt, cfg.hostname);
    break;
#if 0
  case 2: /* "MultPart" */
#if LWIP_HTTPD_SSI_MULTIPART
    switch (current_tag_part) {
    case 0:
      printed = snprintf(pcInsert, iInsertLen, "part0");
      *next_tag_part = 1;
      break;
    case 1:
      printed = snprintf(pcInsert, iInsertLen, "part1");
      *next_tag_part = 2;
      break;
    case 2:
      printed = snprintf(pcInsert, iInsertLen, "part2");
      break;
    default:
      printed = snprintf(pcInsert, iInsertLen, "unhandled part: %d", (int)current_tag_part);
      break;
    }
#else
    printed = snprintf(pcInsert, iInsertLen, "LWIP_HTTPD_SSI_MULTIPART disabled");
#endif
    break;
#if LWIP_HTTPD_EXAMPLE_SSI_SIMPLE_CGI_INTEGRATION
  case 3:
    if (connection_state) {
      char *params = (char *)connection_state;
      if (*params) {
        printed = snprintf(pcInsert, iInsertLen, "%s", (char *)params);
      } else {
        printed = snprintf(pcInsert, iInsertLen, "none");
      }
    } else {
       printed = snprintf(pcInsert, iInsertLen, "NULL");
    }
    break;
#endif
#endif
  default: /* unknown tag */
    printed = 0;
    break;
  }
  LWIP_ASSERT("sane length", printed <= 0xFFFF);
  return (u16_t)printed;
}

static void ssi_ex_init(void)
{
  int i;
  for (i = 0; i < LWIP_ARRAYSIZE(ssi_example_tags); i++) {
    LWIP_ASSERT("tag too long for LWIP_HTTPD_MAX_TAG_NAME_LEN",
      strlen(ssi_example_tags[i]) <= LWIP_HTTPD_MAX_TAG_NAME_LEN);
  }

  http_set_ssi_handler(ssi_example_ssi_handler,
#if LWIP_HTTPD_SSI_RAW
    NULL, 0
#else
    ssi_example_tags, LWIP_ARRAYSIZE(ssi_example_tags)
#endif
    );
}

void *
fs_state_init(struct fs_file *file, const char *name)
{
  char *ret;
  LWIP_UNUSED_ARG(file);
  LWIP_UNUSED_ARG(name);
  ret = (char *)mem_malloc(MAX_CGI_LEN);
  if (ret) {
    *ret = 0;
  }
  return ret;
}

void
fs_state_free(struct fs_file *file, void *state)
{
  LWIP_UNUSED_ARG(file);
  if (state != NULL) {
    mem_free(state);
  }
}

void
httpd_cgi_handler(struct fs_file *file, const char* uri, int iNumParams,
                              char **pcParam, char **pcValue
#if defined(LWIP_HTTPD_FILE_STATE) && LWIP_HTTPD_FILE_STATE
                                     , void *connection_state
#endif /* LWIP_HTTPD_FILE_STATE */
                                     )
{
  LWIP_UNUSED_ARG(file);
  LWIP_UNUSED_ARG(uri);

  (void)pcParam;
  (void)pcValue;

#if defined(LWIP_HTTPD_FILE_STATE) && LWIP_HTTPD_FILE_STATE
  if (connection_state != NULL) {
    char *start = (char *)connection_state;
    char *end = start + MAX_CGI_LEN;
    int i;
    memset(start, 0, MAX_CGI_LEN);
    /* print a string of the arguments: */
    for (i = 0; i < iNumParams; i++) {
      size_t len;
      len = end - start;
      if (len) {
        size_t inlen = strlen(pcParam[i]);
        size_t copylen = LWIP_MIN(inlen, len);
        memcpy(start, pcParam[i], copylen);
        start += copylen;
        len -= copylen;
      }
      if (len) {
        *start = '=';
        start++;
        len--;
      }
      if (len) {
        size_t inlen = strlen(pcValue[i]);
        size_t copylen = LWIP_MIN(inlen, len);
        memcpy(start, pcValue[i], copylen);
        start += copylen;
        len -= copylen;
      }
      if (len) {
        *start = ';';
        len--;
      }
      /* ensure NULL termination */
      end--;
      *end = 0;
    }
  }
#endif /* LWIP_HTTPD_FILE_STATE */
}


/*
 * -------------------------------------------------------------------------------------------------------
 *  End of SSI Support Part
 * -------------------------------------------------------------------------------------------------------
 */

/*
 * -------------------------------------------------------------------------------------------------------
 *  Post Support Part
 * -------------------------------------------------------------------------------------------------------
 */

#define USER_PASS_BUFSIZE 16

static void *current_connection;
static void *valid_connection;
static char last_user[USER_PASS_BUFSIZE];
static uint16_t postPageIndex = 0;

struct post_uri {
  const char *uri;
  uint8_t     len_uri;
  const char *response;
};

enum {
  POST_IDX_LOGIN        =   0,
  POST_IDX_CONFIG       =   1,
  POST_IDX_IP_CGI       =   2,
  POST_IDX_MISC_CGI     =   3,
  POST_IDX_DEFAULT_CGI  =   4,
};

struct post_uri  postPage[] =
{
  { "/login.cgi",     10, "/loginfail.html" },
  { "/config.cgi",    11, "/test.html"},
  /* in misc.shtm */
  { "/ip.cfg",         7, "/test.html"},
  { "/misc.cgi",       9, "/test.html"},
  { "/defaults.cgi",  13, "/test.html"},
};

void dispatchPost(const char *uri, char *response_uri, const u16_t response_uri_len)
{
  const uint8_t pages = (sizeof(postPage)/sizeof(struct post_uri));
  uint8_t i;

  /* not found */
  postPageIndex = 0xFFFF;
  for (i = 0; i < pages; i++) {
     if (memcmp(postPage[i].uri, uri, postPage[i].len_uri) && (strlen(uri) == postPage[i].len_uri)) {
        snprintf(response_uri, response_uri_len, postPage[i].response);
        postPageIndex = i;
       break;
     }
  }

}

err_t
httpd_post_begin(void *connection, const char *uri, const char *http_request,
                 u16_t http_request_len, int content_len, char *response_uri,
                 u16_t response_uri_len, u8_t *post_auto_wnd)
{
  LWIP_UNUSED_ARG(connection);
  LWIP_UNUSED_ARG(http_request);
  LWIP_UNUSED_ARG(http_request_len);
  LWIP_UNUSED_ARG(content_len);
  LWIP_UNUSED_ARG(post_auto_wnd);
  if (current_connection != connection) {
    current_connection = connection;
    valid_connection = NULL;
    dispatchPost(uri, response_uri, response_uri_len);
    /* e.g. for large uploads to slow flash over a fast connection, you should
        manually update the rx window. That way, a sender can only send a full
        tcp window at a time. If this is required, set 'post_aut_wnd' to 0.
        We do not need to throttle upload speed here, so: */
    *post_auto_wnd = 1;
    return ERR_OK;
  }
  return ERR_VAL;
}

err_t
httpd_post_receive_data(void *connection, struct pbuf *p)
{
  if (current_connection == connection) {
    u16_t token_staticip = pbuf_memfind(p, "staticip=", 9, 0);
    // u16_t token_sip = pbuf_memfind(p, "sip=", 4, 0);

    u16_t token_user = pbuf_memfind(p, "user=", 5, 0);
    u16_t token_pass = pbuf_memfind(p, "pass=", 5, 0);
    if (token_staticip != 0xFFFF) {
        printf("find staticip:\n" );
    }

    if ((token_user != 0xFFFF) && (token_pass != 0xFFFF)) {
      u16_t value_user = token_user + 5;
      u16_t value_pass = token_pass + 5;
      u16_t len_user = 0;
      u16_t len_pass = 0;
      u16_t tmp;
      /* find user len */
      tmp = pbuf_memfind(p, "&", 1, value_user);
      if (tmp != 0xFFFF) {
        len_user = tmp - value_user;
      } else {
        len_user = p->tot_len - value_user;
      }
      /* find pass len */
      tmp = pbuf_memfind(p, "&", 1, value_pass);
      if (tmp != 0xFFFF) {
        len_pass = tmp - value_pass;
      } else {
        len_pass = p->tot_len - value_pass;
      }
      if ((len_user > 0) && (len_user < USER_PASS_BUFSIZE) &&
          (len_pass > 0) && (len_pass < USER_PASS_BUFSIZE)) {
        /* provide contiguous storage if p is a chained pbuf */
        char buf_user[USER_PASS_BUFSIZE];
        char buf_pass[USER_PASS_BUFSIZE];
        char *user = (char *)pbuf_get_contiguous(p, buf_user, sizeof(buf_user), len_user, value_user);
        char *pass = (char *)pbuf_get_contiguous(p, buf_pass, sizeof(buf_pass), len_pass, value_pass);
        if (user && pass) {
          user[len_user] = 0;
          pass[len_pass] = 0;
          if (!strcmp(user, "lwip") && !strcmp(pass, "post")) {
            /* user and password are correct, create a "session" */
            valid_connection = connection;
            memcpy(last_user, user, sizeof(last_user));
          }
        }
      }
    }
    /* not returning ERR_OK aborts the connection, so return ERR_OK unless the
       conenction is unknown */
    return ERR_OK;
  }
  return ERR_VAL;
}

void
httpd_post_finished(void *connection, char *response_uri, u16_t response_uri_len)
{
  /* default page is "login failed" */
  snprintf(response_uri, response_uri_len, "/loginfail.html");
  if (current_connection == connection) {
    printf("post finished\n");
    if (valid_connection == connection) {
      /* login succeeded */
      snprintf(response_uri, response_uri_len, "/session.html");
    }
    current_connection = NULL;
    valid_connection = NULL;
  }
}

/*
 * -------------------------------------------------------------------------------------------------------
 *  End of Post Support Part
 * -------------------------------------------------------------------------------------------------------
 */
void httpd_ext_init(void)
{
  cgi_ex_init();
  ssi_ex_init();
}
