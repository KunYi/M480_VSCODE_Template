
#include "NuMicro.h"
#include "main.h"

uint32_t convPortDataLen(const uint32_t v)
{
    switch (v) {
    case 5:
        return UART_WORD_LEN_5;
    case 6:
        return UART_WORD_LEN_6;
    case 7:
        return UART_WORD_LEN_7;
    case 8:
        return UART_WORD_LEN_8;
    default:
        MYASSERT(0, "error! data length\n");
    }
    return UART_WORD_LEN_8;
}

uint32_t convPortParity(const uint32_t v)
{
    switch (v) {
    case PARITY_NONE:
        return UART_PARITY_NONE;
    case PARITY_ODD:
        return UART_PARITY_ODD;
    case PARITY_EVEN:
        return UART_PARITY_EVEN;
    case PARITY_MARK:
        return UART_PARITY_MARK;
    case PARITY_SPACE:
        return UART_PARITY_SPACE;
    default:
        MYASSERT(0, "error! parity\n");
    }
    return UART_PARITY_NONE;
}

uint32_t convPortStopBits(const uint32_t v)
{
    switch (v) {
    case STOPBIT_1:
        return UART_STOP_BIT_1;
    case STOPBIT_1D5:
        return UART_STOP_BIT_1_5;
    case STOPBIT_2:
        return UART_STOP_BIT_2;
    default:
        MYASSERT(0, "error! stopbits\n");
    }
    return UART_STOP_BIT_1;
}

void UART1_IRQHandler(void)
{
  const uint32_t u32IntSts = UART1->INTSTS;
  const uint32_t u32DAT = UART1->DAT; // read out data;

  if (u32IntSts & UART_INTSTS_HWRLSIF_Msk)
  {
    if (UART1->FIFOSTS & UART_FIFOSTS_BIF_Msk)
      printf("\n BIF \n");
    if(UART1->FIFOSTS & UART_FIFOSTS_FEF_Msk)
      printf("\n FEF \n");
    if(UART1->FIFOSTS & UART_FIFOSTS_PEF_Msk)
      printf("\n PEF \n");

    printf("\n Error Data is '0x%lx' \n", u32DAT);
    UART1->FIFOSTS = (UART_FIFOSTS_BIF_Msk | UART_FIFOSTS_FEF_Msk | UART_FIFOSTS_PEF_Msk);
  }
}

void UART5_IRQHandler(void)
{
  const uint32_t u32IntSts = UART5->INTSTS;
  const uint32_t u32DAT = UART5->DAT; // read out data;

  if (u32IntSts & UART_INTSTS_HWRLSIF_Msk)
  {
    if (UART5->FIFOSTS & UART_FIFOSTS_BIF_Msk)
      printf("\n BIF \n");
    if(UART5->FIFOSTS & UART_FIFOSTS_FEF_Msk)
      printf("\n FEF \n");
    if(UART5->FIFOSTS & UART_FIFOSTS_PEF_Msk)
      printf("\n PEF \n");

    printf("\n Error Data is '0x%lx' \n", u32DAT);
    UART5->FIFOSTS = (UART_FIFOSTS_BIF_Msk | UART_FIFOSTS_FEF_Msk | UART_FIFOSTS_PEF_Msk);
  }
}
