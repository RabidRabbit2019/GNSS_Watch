#include "gnss.h"
#include "display.h"

#include "n200_func.h"
#include "gd32vf103.h"
#include <font_22_24.h>

#include <stddef.h>
#include <string.h>


#define RX_STATUS_NONE      0
#define RX_STATUS_HALF      1
#define RX_STATUS_FULL      2
#define RX_STATUS_RESTART   3

void *memmem(const void *h0, size_t k, const void *n0, size_t l);

static char g_rx_buffer[4096];

static volatile int g_rx_status = RX_STATUS_NONE;


static void process_GNSS_data( char * a_from, size_t a_size ) {
  char * v_ptr = (char *)memmem( (const void *)a_from, a_size, (const void *)"$GNRMC", 6u );
  if ( !v_ptr && (v_ptr - a_from) <= ((sizeof(g_rx_buffer) / 2) - 18) ) {
    v_ptr[17] = '\0';
    display_write_string( 0, 140, v_ptr + 7, &font_22_24_font, DISPLAY_COLOR_YELLOW, DISPLAY_COLOR_BLACK );    
  }
}


void init_GNSS() {
  // remap USART0 (PB7 - RX, PB6 - TX)
  AFIO_PCF0 |= AFIO_PCF0_USART0_REMAP;
  // configure USART pins
  // PB7 input floating, PB6 output push-pull 2MHz alternate fn
  GPIO_CTL0(GPIOB) = (GPIO_CTL0(GPIOB) & ~(GPIO_MODE_MASK0(7) | GPIO_MODE_MASK0(6)))
                   | GPIO_MODE_SET0(7, 0x0F & GPIO_MODE_IN_FLOATING)
                   | GPIO_MODE_SET0(6, 0x0F & (GPIO_MODE_AF_PP | GPIO_OSPEED_2MHZ))
                   ;
  // configure DMA
  DMA_CH4PADDR(DMA0) = (uint32_t)&(USART_DATA(USART0));
  DMA_CH4MADDR(DMA0) = (uint32_t)g_rx_buffer;
  // 
  DMA_INTC(DMA0) = DMA_FLAG_ADD(DMA_FLAG_ERR, DMA_CH4)
                 | DMA_FLAG_ADD(DMA_FLAG_FTF, DMA_CH4)
                 | DMA_FLAG_ADD(DMA_FLAG_HTF, DMA_CH4)
                 | DMA_FLAG_ADD(DMA_FLAG_G, DMA_CH4)
                 ;
  DMA_CH4CNT(DMA0) = (uint32_t)sizeof(g_rx_buffer);
  DMA_CH4CTL(DMA0) = DMA_CHXCTL_MNAGA | DMA_CHXCTL_CHEN;
  // configure USART0, 9600 8N1 no parity
  USART_CTL0(USART0) = 0;
  USART_BAUD(USART0) = (416u << 4) | 11u; // 64000000 / 16 / (416 + 11/16) = 9599.531
  char v_buf[2];
  v_buf[1] = '\0';
  for (;;) {
    while ( 0 == (USART_STAT(USART0) & USART_STAT_RBNE) ) {}
    v_buf[0] = (char)USART_DATA(USART0);
    if ( v_buf[0] >= ' ' ) {
      display_write_string( 0, 140, v_buf, &font_22_24_font, DISPLAY_COLOR_YELLOW, DISPLAY_COLOR_BLACK );
    }
  }
  //USART_CTL2(USART0) = USART_CTL2_DENR;
  USART_CTL0(USART0) = USART_CTL0_UEN | USART_CTL0_REN;
  // at this point USART0 received bytes stores into g_rx_buffer by DMA
  // Set 'vector mode' so the interrupt uses the vector table.
  eclic_set_vmode( DMA0_Channel4_IRQn );
  // Enable the interrupt with low priority and 'level'.
  eclic_set_irq_lvl_abs( DMA0_Channel4_IRQn, 1 );
  eclic_set_irq_priority( DMA0_Channel4_IRQn, 1 );
  eclic_enable_interrupt( DMA0_Channel4_IRQn );
}


void time_slice_GNSS() {
  switch ( g_rx_status ) {
    case RX_STATUS_HALF:
      g_rx_status = RX_STATUS_NONE;
      process_GNSS_data( g_rx_buffer, sizeof(g_rx_buffer) / 2 );
      break;
      
    case RX_STATUS_FULL:
      g_rx_status = RX_STATUS_NONE;
      process_GNSS_data( g_rx_buffer + (sizeof(g_rx_buffer) / 2), sizeof(g_rx_buffer) / 2 );
      break;
      
    case RX_STATUS_RESTART:
      g_rx_status = RX_STATUS_NONE;
      USART_DATA(USART0);
      DMA_CH4CTL(DMA0) &= ~DMA_CHXCTL_CHEN;
      DMA_INTC(DMA0) = DMA_FLAG_ADD(DMA_FLAG_ERR, DMA_CH4)
                     | DMA_FLAG_ADD(DMA_FLAG_FTF, DMA_CH4)
                     | DMA_FLAG_ADD(DMA_FLAG_HTF, DMA_CH4)
                     | DMA_FLAG_ADD(DMA_FLAG_G, DMA_CH4)
                     ;
      DMA_CH4MADDR(DMA0) = (uint32_t)g_rx_buffer;
      DMA_CH4CNT(DMA0) = (uint32_t)sizeof(g_rx_buffer);
      DMA_CH4CTL(DMA0) = DMA_CHXCTL_MNAGA | DMA_CHXCTL_CHEN;
      break;
      
    default:
      break;
  }
}


__attribute__ ((interrupt))
void DMA0_Channel4_IRQHandler(void) {
  uint32_t v_dma_status = DMA_INTF(DMA0);
  
  if ( 0 != (DMA_FLAG_ADD(DMA_FLAG_ERR, DMA_CH4) & v_dma_status) ) {
    // error
    // disable channel
    DMA_CH4CTL(DMA0) = 0;
    // set status for restart
    g_rx_status = RX_STATUS_RESTART;
    //
    return;
  }
  if ( 0 != (DMA_FLAG_ADD(DMA_FLAG_HTF, DMA_CH4) & v_dma_status) ) {
    // first half of buffer completed
    g_rx_status = RX_STATUS_HALF;
  } else {
    // second half of buffer completed
    g_rx_status = RX_STATUS_FULL;
    // restart channel
    DMA_CH4CTL(DMA0) &= ~DMA_CHXCTL_CHEN;
    DMA_INTC(DMA0) = DMA_FLAG_ADD(DMA_FLAG_ERR, DMA_CH4)
                   | DMA_FLAG_ADD(DMA_FLAG_FTF, DMA_CH4)
                   | DMA_FLAG_ADD(DMA_FLAG_HTF, DMA_CH4)
                   | DMA_FLAG_ADD(DMA_FLAG_G, DMA_CH4)
                   ;
    DMA_CH4MADDR(DMA0) = (uint32_t)g_rx_buffer;
    DMA_CH4CNT(DMA0) = (uint32_t)sizeof(g_rx_buffer);
    DMA_CH4CTL(DMA0) = DMA_CHXCTL_MNAGA | DMA_CHXCTL_CHEN;
  }
}
