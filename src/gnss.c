#include "gnss.h"
#include "display.h"
#include "rtc.h"

#include "n200_func.h"
#include "gd32vf103.h"

#include <stddef.h>
#include <string.h>
#include <time.h>


#define CLOCK_SYNC_INTERVAL 3600

#define RX_STATUS_NONE      0
#define RX_STATUS_HALF      1
#define RX_STATUS_FULL      2
#define RX_STATUS_RESTART   3


// current unix time
time_t g_time = UTS_2024_01_01_00_00_00;
// last time sync
static time_t g_last_sync_time = 0;
// signal ok
bool g_gnss_ok = false;

// timezone UTC +03:00
time_t g_time_zone = 3 * 3600;

// buffer for receive data from GNSS module
static char g_rx_buffer[4096];

// data receive status (via UART)
static volatile int g_rx_status = RX_STATUS_NONE;

// $GNRMC,195959.000,V,,,,,,,160124,,,N,V*21\r\n

// process received data
static void process_GNSS_data( char * a_from, size_t a_size ) {
  a_from[a_size - 1] = '\0';
  
  char * v_ptr = strstr( a_from, "$GNRMC" );
  char * v_end = NULL;
  
  if ( v_ptr ) {
    v_end = strstr( v_ptr +  6, "\r\n" );
    if ( v_end ) {
      v_end[0] = '\0';
      
      int v_parts = 0;
      int v_token_ndx = 0;
      struct tm v_tm;
      bzero( &v_tm, sizeof(v_tm) );
      
      for ( char * v_token = strsep( &v_ptr, "," ); v_token; v_token = strsep( &v_ptr, "," ) ) {
        switch (v_token_ndx) {
          case 1:
            // time
            if ( strlen( v_token ) >= 6 ) {
              v_tm.tm_hour = (v_token[0] - '0') * 10 + (v_token[1] - '0');
              v_tm.tm_min  = (v_token[2] - '0') * 10 + (v_token[3] - '0');
              v_tm.tm_sec  = (v_token[4] - '0') * 10 + (v_token[5] - '0');
              ++v_parts;
            }
            break;
          
          case 9:
            // date
            if ( strlen( v_token ) >= 6 ) {
              v_tm.tm_mday = (v_token[0] - '0') * 10 + (v_token[1] - '0');
              v_tm.tm_mon  = (v_token[2] - '0') * 10 + (v_token[3] - '0') - 1;
              v_tm.tm_year = (v_token[4] - '0') * 10 + (v_token[5] - '0') + 100;
              ++v_parts;
            }
            break;
            
          default:
            break;
        }
        ++v_token_ndx;
      }
      
      // signal flag
      g_gnss_ok = (2 == v_parts);
      // 
      if ( g_gnss_ok ) {
        // проверим не пора ли обновить локальную дату/время
        if ( (g_time - g_last_sync_time) > CLOCK_SYNC_INTERVAL ) {
          g_time = timegm( &v_tm );
          if ( g_rtc_initialized ) {
            rtc_set_cnt( g_time );
          }
          g_last_sync_time = g_time;
        }
      } else {
        // next received data from GNSS updates clock
        g_last_sync_time = 0;
      }
    }
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
  DMA_CH4CTL(DMA0) = 0;
  DMA_CH4PADDR(DMA0) = (uint32_t)&(USART_DATA(USART0));
  DMA_CH4MADDR(DMA0) = (uint32_t)g_rx_buffer;
  // 
  DMA_INTC(DMA0) = DMA_FLAG_ADD(DMA_FLAG_ERR, DMA_CH4)
                 | DMA_FLAG_ADD(DMA_FLAG_FTF, DMA_CH4)
                 | DMA_FLAG_ADD(DMA_FLAG_HTF, DMA_CH4)
                 ;
  DMA_CH4CNT(DMA0) = (uint32_t)sizeof(g_rx_buffer);
  DMA_CH4CTL(DMA0) = DMA_CHXCTL_HTFIE
                   | DMA_CHXCTL_FTFIE
                   | DMA_CHXCTL_ERRIE
                   | DMA_CHXCTL_MNAGA
                   | DMA_CHXCTL_CHEN
                   ;
  // configure USART0, 9600 8N1 no parity
  USART_CTL0(USART0) = 0;
  USART_BAUD(USART0) = (416u << 4) | 11u; // 64000000 / 16 / (416 + 11/16) = 9599.531
  USART_CTL2(USART0) = USART_CTL2_DENR;
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
      DMA_CH4CTL(DMA0) = 0;
      DMA_INTC(DMA0) = DMA_FLAG_ADD(DMA_FLAG_ERR, DMA_CH4)
                     | DMA_FLAG_ADD(DMA_FLAG_FTF, DMA_CH4)
                     | DMA_FLAG_ADD(DMA_FLAG_HTF, DMA_CH4)
                     ;
      DMA_CH4MADDR(DMA0) = (uint32_t)g_rx_buffer;
      DMA_CH4CNT(DMA0) = (uint32_t)sizeof(g_rx_buffer);
      DMA_CH4CTL(DMA0) = DMA_CHXCTL_HTFIE
                       | DMA_CHXCTL_FTFIE
                       | DMA_CHXCTL_ERRIE
                       | DMA_CHXCTL_MNAGA
                       | DMA_CHXCTL_CHEN
                       ;
      break;
      
    default:
      break;
  }
}


__attribute__ ((interrupt))
void DMA0_Channel4_IRQHandler(void) {
  uint32_t v_dma_status = DMA_INTF(DMA0);
  // прерывания от DMA по трём флагам HTF, FTF и ERR
  if ( 0 != (DMA_FLAG_ADD(DMA_FLAG_ERR, DMA_CH4) & v_dma_status) ) {
    // error
    // disable channel
    DMA_CH4CTL(DMA0) = 0;
    DMA_INTC(DMA0) = DMA_FLAG_ADD(DMA_FLAG_ERR, DMA_CH4)
                   | DMA_FLAG_ADD(DMA_FLAG_FTF, DMA_CH4)
                   | DMA_FLAG_ADD(DMA_FLAG_HTF, DMA_CH4)
                   ;
    // set status for restart
    g_rx_status = RX_STATUS_RESTART;
  } else {
    if ( 0 != (DMA_FLAG_ADD(DMA_FLAG_HTF, DMA_CH4) & v_dma_status) ) {
      DMA_INTC(DMA0) = DMA_FLAG_ADD(DMA_FLAG_HTF, DMA_CH4);
      // first half of buffer completed
      g_rx_status = RX_STATUS_HALF;
    } else {
      // restart channel
      DMA_CH4CTL(DMA0) = 0;
      DMA_INTC(DMA0) = DMA_FLAG_ADD(DMA_FLAG_FTF, DMA_CH4);
      DMA_CH4MADDR(DMA0) = (uint32_t)g_rx_buffer;
      DMA_CH4CNT(DMA0) = (uint32_t)sizeof(g_rx_buffer);
      DMA_CH4CTL(DMA0) = DMA_CHXCTL_HTFIE
                       | DMA_CHXCTL_FTFIE
                       | DMA_CHXCTL_ERRIE
                       | DMA_CHXCTL_MNAGA
                       | DMA_CHXCTL_CHEN
                       ;
      // second half of buffer completed
      g_rx_status = RX_STATUS_FULL;
    }
  }
}
