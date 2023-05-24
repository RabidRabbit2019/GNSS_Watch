#include "display.h"

#include "n200_func.h"
#include "gd32vf103.h"


#define ILI9341_RESET           0x01
#define ILI9341_SLEEP_OUT       0x11
#define ILI9341_GAMMA				    0x26
#define ILI9341_DISPLAY_OFF     0x28
#define ILI9341_DISPLAY_ON      0x29
#define ILI9341_COLUMN_ADDR     0x2A
#define ILI9341_PAGE_ADDR       0x2B
#define ILI9341_GRAM            0x2C
#define ILI9341_MAC             0x36
#define ILI9341_PIXEL_FORMAT    0x3A
#define ILI9341_WDB             0x51
#define ILI9341_WCD             0x53
#define ILI9341_RGB_INTERFACE   0xB0
#define ILI9341_FRC             0xB1
#define ILI9341_BPC             0xB5
#define ILI9341_DFC             0xB6
#define ILI9341_POWER1          0xC0
#define ILI9341_POWER2          0xC1
#define ILI9341_VCOM1           0xC5
#define ILI9341_VCOM2           0xC7
#define ILI9341_POWERA          0xCB
#define ILI9341_POWERB          0xCF
#define ILI9341_PGAMMA          0xE0
#define ILI9341_NGAMMA          0xE1
#define ILI9341_DTCA            0xE8
#define ILI9341_DTCB            0xEA
#define ILI9341_POWER_SEQ       0xED
#define ILI9341_3GAMMA_EN       0xF2
#define ILI9341_INTERFACE       0xF6
#define ILI9341_PRC             0xF7


const uint8_t g_ili9341_init[] = {
  0x09, 0x2C, 0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 // ILI9342_GRAM
, 0x05, 0xCB, 0x39, 0x2C, 0x00, 0x34, 0x02 // ILI9341_POWERA
, 0x03, 0xCF, 0x00, 0xC1, 0x30 // ILI9341_POWERB
, 0x03, 0xE8, 0x85, 0x00, 0x78 // ILI9341_DTCA
, 0x02, 0xEA, 0x00, 0x00 // ILI9341_DTCB
, 0x04, 0xED, 0x64, 0x03, 0x12, 0x81 // ILI9341_POWER_SEQ
, 0x01, 0xF7, 0x20 // ILI9341_PRC
, 0x01, 0xC0, 0x23 // ILI9341_POWER1
, 0x01, 0xC1, 0x10 // ILI9341_POWER2
, 0x02, 0xC5, 0x3E, 0x28 // ILI9341_VCOM1
, 0x01, 0xC7, 0x86 // ILI9341_VCOM2
, 0x01, 0x36, 0x08 // ILI9341_MAC
, 0x01, 0x3A, 0x55 // ILI9341_PIXEL_FORMAT
, 0x02, 0xB1, 0x00, 0x18 // ILI9341_FRC
, 0x03, 0xB6, 0x08, 0x82, 0x27 // ILI9341_DFC
, 0x01, 0xF2, 0x00 // ILI9341_3GAMMA_EN
, 0x04, 0x2A, 0x00, 0x00, 0x00, 0xEF // ILI9341_COLUMN_ADDR
, 0x04, 0x2B, 0x00, 0x00, 0x01, 0x3F // ILI9341_PAGE_ADDR
, 0x01, 0x26, 0x01 // ILI9341_GAMMA
, 0x0F, 0xE0, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00 // ILI9341_PGAMMA
, 0x0F, 0xE1, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F // ILI9341_NGAMMA
, 0x00 // end marker
};


extern volatile uint32_t g_milliseconds;
void delay_ms( uint32_t a_ms );


static void display_select() {
  GPIO_BOP(GPIOA) = GPIO_BOP_CR4;
}


static void display_deselect() {
  GPIO_BOP(GPIOA) = GPIO_BOP_BOP4;
}


static void display_cmd_mode() {
  GPIO_BOP(GPIOA) = GPIO_BOP_CR3;
}


static void display_data_mode() {
  GPIO_BOP(GPIOA) = GPIO_BOP_BOP3;
}


static void display_reset() {
  GPIO_BOP(GPIOA) = GPIO_BOP_CR0;
  delay_ms(50);
  GPIO_BOP(GPIOA) = GPIO_BOP_BOP0;
  delay_ms(150);
}


uint8_t g_dummy = 0;

static void display_spi_write_start( const uint8_t * a_src, uint32_t a_size ) {
  // read data register
  SPI_DATA(SPI0);
  // clear flags for channels 1 and 2
  DMA_INTC(DMA0) = DMA_FLAG_ADD(DMA_FLAG_ERR, 1)
                 | DMA_FLAG_ADD(DMA_FLAG_FTF, 1)
                 | DMA_FLAG_ADD(DMA_FLAG_HTF, 1)
                 | DMA_FLAG_ADD(DMA_FLAG_G, 1)
                 | DMA_FLAG_ADD(DMA_FLAG_ERR, 2)
                 | DMA_FLAG_ADD(DMA_FLAG_FTF, 2)
                 | DMA_FLAG_ADD(DMA_FLAG_HTF, 2)
                 | DMA_FLAG_ADD(DMA_FLAG_G, 2)
                 ;
  // enable DMA0 channel 1 for receive
  DMA_CH1MADDR(DMA0) = (uint32_t)&g_dummy;
  DMA_CH1CNT(DMA0) = a_size;
  DMA_CH1CTL(DMA0) = DMA_CHXCTL_CHEN;
  // enable DMA0 channel 2 for transmit
  DMA_CH2MADDR(DMA0) = (uint32_t)a_src;
  DMA_CH2CNT(DMA0) = a_size;
  DMA_CH2CTL(DMA0) = DMA_CHXCTL_DIR
                   | DMA_CHXCTL_MNAGA
                   | DMA_CHXCTL_CHEN
                   ;
}


static void display_spi_write_end() {
  uint32_t v_from = g_milliseconds;
  // wait for receive complete (all data transferred) or error at any channel
  while ( ((uint32_t)(g_milliseconds - v_from)) < 50u ) {
    if ( 0 != (DMA_INTF(DMA0) & (DMA_FLAG_ADD(DMA_FLAG_ERR, 1) | DMA_FLAG_ADD(DMA_FLAG_ERR, 2) | DMA_FLAG_ADD(DMA_FLAG_FTF, 1))) ) {
      break;
    }
  }
  // disable used channels
  DMA_CH1CTL(DMA0) &= ~DMA_CHXCTL_CHEN;
  DMA_CH2CTL(DMA0) &= ~DMA_CHXCTL_CHEN;
}


// syncronized data write (return when transfer completed)
static void display_spi_write( const uint8_t * a_src, uint32_t a_size ) {
  display_spi_write_start( a_src, a_size );
  display_spi_write_end();
}


static void display_write_cmd( uint8_t a_cmd ) {
  display_cmd_mode();
  display_spi_write( &a_cmd, sizeof(a_cmd) );
}


static void display_write_data( const uint8_t * a_buff, uint32_t a_buff_size ) {
  display_data_mode();
  display_spi_write( a_buff, a_buff_size );
}


// initialize display, ILI9341, 320x240, spi four lines
void display_init() {
  // enable SPI0 clocks
  RCU_APB2EN |= RCU_APB2EN_SPI0EN;
  // SPI clock = APB2 clock / 4 (64 / 4 = 16 MHz), master mode
  SPI_CTL0(SPI0) = SPI_CTL0_SWNSSEN
                 | SPI_CTL0_SWNSS
                 | SPI_CTL0_SPIEN
                 | SPI_PSC_4
                 | SPI_CTL0_MSTMOD
                 ;
  // prepare DMA for SPI0
  // enable DMA0 clocks 
  RCU_AHBEN |= RCU_AHBEN_DMA0EN;
  // enable receive and transmit
  SPI_CTL1(SPI0) = SPI_CTL1_DMAREN | SPI_CTL1_DMATEN;
  DMA_CH1PADDR(DMA0) = (uint32_t)&(SPI_DATA(SPI0));
  DMA_CH2PADDR(DMA0) = (uint32_t)&(SPI_DATA(SPI0));
  // reset sequence
  delay_ms(125);
  display_select();
  display_write_cmd(ILI9341_RESET);
  delay_ms(125);
  // issue init commands set
  for ( const uint8_t * v_ptr = g_ili9341_init; *v_ptr; ++v_ptr ) {
    uint32_t v_cnt = *v_ptr++;
    uint8_t v_cmd = *v_ptr++;
    
    display_write_cmd( v_cmd );
    display_write_data( v_ptr, v_cnt );
  }
  display_write_cmd(ILI9341_SLEEP_OUT);
  delay_ms(6);
  display_write_cmd(ILI9341_DISPLAY_ON);
  display_deselect();
}
