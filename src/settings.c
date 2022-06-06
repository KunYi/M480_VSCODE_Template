#include "NuMicro.h"
#include "settings.h"
#include "utils.h"
#include <string.h>

#define FLASH_END         FMC_APROM_SIZE
#define FLASH_SIZE        FMC_FLASH_PAGE_SIZE

// last 2 pages for DATA space Flash
#define DATA_FLASH_START  (FLASH_END - (FMC_FLASH_PAGE_SIZE * 2))
// last page for configuration
#define CONFIG_FLASH_START (FLASH_END - FMC_FLASH_PAGE_SIZE)

static uint8_t mac_addr[6] = {0x00, 0x00, 0x00, 0x55, 0x66, 0x77};
struct Configuration cfg;

static int  set_data_flash_base(uint32_t u32DFBA)
{
  uint32_t   au32Config[2];          /* User Configuration */

  /* Read User Configuration 0 & 1 */
  if (FMC_ReadConfig(au32Config, 2) < 0) {
    MYASSERT(0, "\nRead User Config failed!\n");
    return -1;                     /* failed to read User Configuration */
  }

  /* Check if Data Flash is enabled and is expected address. */
  if ((!(au32Config[0] & 0x1)) && (au32Config[1] == u32DFBA))
    return 0;                      /* no need to modify User Configuration */

  FMC_ENABLE_CFG_UPDATE();           /* Enable User Configuration update. */

  au32Config[0] &= ~0x1;             /* Clear CONFIG0 bit 0 to enable Data Flash */
  au32Config[1] = u32DFBA;           /* Give Data Flash base address  */

  /* Update User Configuration settings. */
  if (FMC_WriteConfig(au32Config, 2) < 0)
    return -1;                     /* failed to write user configuration */

  /* Perform chip reset to make new User Config take effect. */
  SYS->IPRST0 = SYS_IPRST0_CHIPRST_Msk;
  return 0;                          /* success */
}

void initCfgStruct(struct Configuration *pCfg)
{
  struct PortSettings *port = NULL;

  memset(pCfg, 0xff, sizeof(struct Configuration));
  memset(pCfg->hostname, 0, sizeof(pCfg->hostname));

  memcpy(pCfg->macaddr, mac_addr, 6);
  pCfg->ip = makeIP(192, 168, 10, 120);
  pCfg->netmask = makeIP(255, 255, 255, 0);
  pCfg->static_ip = 1;
  pCfg->inUse = CFG_DATABLOCK_INUSE;
  for (int i = 0; i < SUPPORT_PORTS; i++) {
    port = &pCfg->port[i];
    port->baudrate = 115200;
    port->datalength = 8;
    port->parity = PARITY_NONE;
    port->stopbits = STOPBIT_1;
    port->flowcontrol = FLOWCONTROL_NONE;
  }
  pCfg->num = 0;
  strcpy(pCfg->hostname, "UCOMM");
  pCfg->crc = hwCalcCRC16((const uint8_t *)pCfg, SIZE_CONFIGURATION - 2, 0);
}

static int32_t writeConfig(int num, struct Configuration *pCfg)
{
  uint32_t addr =  CONFIG_FLASH_START + (SIZE_CONFIGURATION * num);
  uint32_t *pDat = (uint32_t *)pCfg;
  int32_t result = 0;
  for (uint32_t i = 0; i < (SIZE_CONFIGURATION / 8); i++) {
    int32_t ret = FMC_Write8Bytes(addr, pDat[0], pDat[1]);
    if (ret < 0)
      break;
    result += 8;
    addr += 8;
    pDat += 2;
  }
  MYASSERT(result == SIZE_CONFIGURATION, "WrieCofiguration, %ld\n", result);
  return result;
}

static int chkCfgBlockIsBlank(void)
{
  uint32_t startAddr = CONFIG_FLASH_START;
  uint32_t buff[(SIZE_CONFIGURATION/sizeof(uint32_t))];

  for (int i = 0; i < (FLASH_SIZE/SIZE_CONFIGURATION); i++) {
    memcpy(buff, (const void*)startAddr, SIZE_CONFIGURATION);
    for (int j = 0; j < (SIZE_CONFIGURATION/sizeof(uint32_t)); j++) {
      if (buff[j] != 0xFFFFFFFF)
        return -1;
    }
    startAddr += SIZE_CONFIGURATION;
  }
  return 0;
}

static void eraseCfgBlock(void)
{
  FMC_Erase(CONFIG_FLASH_START);
}

static int findValidCfg(struct Configuration *pCfg)
{
  uint32_t startAddr = CONFIG_FLASH_START;
  int  validNum = -1;
  uint8_t  buff[SIZE_CONFIGURATION];
  const struct Configuration *p = (struct Configuration *)buff;

  MYASSERT(pCfg != NULL, "need a buffer\n");

  // find last valid config data
  for (int i = 0; i < (FLASH_SIZE/SIZE_CONFIGURATION); i++) {
    memcpy(buff, (const void*)startAddr, SIZE_CONFIGURATION);
    if ((p->inUse == CFG_DATABLOCK_INUSE) &&
        (p->crc == hwCalcCRC16((const uint8_t *)p, SIZE_CONFIGURATION - 2, 0))) {
      validNum = (int)p->num;
    }
    else {
      if (validNum != -1)
        break;
    }
    startAddr += SIZE_CONFIGURATION;
  }

  if (validNum != -1) {
    startAddr = CONFIG_FLASH_START + (SIZE_CONFIGURATION * validNum);
    memcpy(pCfg, (const void*)startAddr, SIZE_CONFIGURATION);
  }

  return validNum;
}

uint32_t updateDevCfg(void)
{
  cfg.num++;
  if (cfg.num >= (FLASH_SIZE/SIZE_CONFIGURATION))
  {
    cfg.num = 0;
    eraseCfgBlock();
  }

  writeConfig(cfg.num, &cfg);
  return 0;
}

uint32_t readDevCfg(void)
{
  MYASSERT(sizeof(struct Configuration) == SIZE_CONFIGURATION,
        "configuration size must equal 256bytes, current size:%d\n",
        sizeof(struct Configuration));

  /* Unlock register lock protect */
  /* Enable FMC ISP function */
  SYS_UnlockReg();
  FMC_Open();
  set_data_flash_base(DATA_FLASH_START);

  if (findValidCfg(&cfg) == -1) {
    initCfgStruct(&cfg);
    if (chkCfgBlockIsBlank() < 0) {
        eraseCfgBlock();
    }
    writeConfig(0, &cfg);
  }

  FMC_Close();
  /* Lock protected registers */
  SYS_LockReg();
  return 0;
}
