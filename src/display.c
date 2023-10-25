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


#define ILI9341_MAC_MY  0x80
#define ILI9341_MAC_MX  0x40
#define ILI9341_MAC_MV  0x20
#define ILI9341_MAC_ML  0x10
#define ILI9341_MAC_RGB 0x00
#define ILI9341_MAC_BGR 0x08
#define ILI9341_MAC_MH  0x04



const uint8_t g_ili9341_init[] = {
  0x05, 0xCB, 0x39, 0x2C, 0x00, 0x34, 0x02 // ILI9341_POWERA
, 0x03, 0xCF, 0x00, 0xC1, 0x30 // ILI9341_POWERB
, 0x03, 0xE8, 0x85, 0x00, 0x78 // ILI9341_DTCA
, 0x02, 0xEA, 0x00, 0x00 // ILI9341_DTCB
, 0x04, 0xED, 0x64, 0x03, 0x12, 0x81 // ILI9341_POWER_SEQ
, 0x01, 0xF7, 0x20 // ILI9341_PRC
, 0x01, 0xC0, 0x23 // ILI9341_POWER1
, 0x01, 0xC1, 0x10 // ILI9341_POWER2
, 0x02, 0xC5, 0x3E, 0x28 // ILI9341_VCOM1
, 0x01, 0xC7, 0x86 // ILI9341_VCOM2
, 0x01, 0x36, ILI9341_MAC_MV | ILI9341_MAC_BGR // ILI9341_MAC
, 0x01, 0x3A, 0x55 // ILI9341_PIXEL_FORMAT
, 0x02, 0xB1, 0x00, 0x18 // ILI9341_FRC
, 0x03, 0xB6, 0x08, 0x82, 0x27 // ILI9341_DFC
, 0x01, 0xF2, 0x02 // ILI9341_3GAMMA_EN
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
  while ( ((uint32_t)(g_milliseconds - v_from)) < 250u ) {
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


static void display_bit_delay() {
  volatile int i;
  for ( i = 0; i < 10; ++i ) {}
}


// syncronized data write (return when transfer completed)
static void display_write( const uint8_t * a_src, uint32_t a_size ) {
  while ( a_size ) {
    uint8_t b = *a_src;
    for ( int i = 0; i < 8; ++i ) {
      if ( 0 == (0x80 & b) ) {
        // 0
        GPIO_BOP(GPIOA) = GPIO_BOP_CR7;
      } else {
        // 1
        GPIO_BOP(GPIOA) = GPIO_BOP_BOP7;
      }
      display_bit_delay();
      // rise SCK
      GPIO_BOP(GPIOA) = GPIO_BOP_BOP5;
      display_bit_delay();
      // drop SCK
      GPIO_BOP(GPIOA) = GPIO_BOP_CR5;
      // shift next bit
      b <<= 1;
    }
    ++a_src;
    --a_size;
  }
}


static void display_write_cmd_dma( uint8_t a_cmd ) {
  display_cmd_mode();
  display_spi_write( &a_cmd, sizeof(a_cmd) );
}


static void display_write_data_dma( const uint8_t * a_buff, uint32_t a_buff_size ) {
  display_data_mode();
  display_spi_write( a_buff, a_buff_size );
}


static void display_write_cmd( uint8_t a_cmd ) {
  display_cmd_mode();
  display_write( &a_cmd, sizeof(a_cmd) );
}


static void display_write_data( const uint8_t * a_buff, uint32_t a_buff_size ) {
  display_data_mode();
  display_write( a_buff, a_buff_size );
}


static void display_reset() {
  display_write_cmd( ILI9341_RESET );
}


static void display_set_addr_window( uint16_t x, uint16_t y, uint16_t w, uint16_t h ) {
  uint8_t v_cmd_data[4];
  // data for X
  v_cmd_data[0] = x >> 8;
  v_cmd_data[1] = x;
  x += w - 1;
  v_cmd_data[2] = x >> 8;
  v_cmd_data[3] = x;
  // write
  display_write_cmd( ILI9341_COLUMN_ADDR );
  display_write_data( v_cmd_data, sizeof(v_cmd_data) );
  // data for Y
  v_cmd_data[0] = y >> 8;
  v_cmd_data[1] = y;
  y += h - 1;
  v_cmd_data[2] = y >> 8;
  v_cmd_data[3] = y;
  // write
  display_write_cmd( ILI9341_PAGE_ADDR );
  display_write_data( v_cmd_data, sizeof(v_cmd_data) );
  //
  display_write_cmd( ILI9341_GRAM );
}


void display_fill_rectangle( uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    // clipping
    if((x >= DISPLAY_WIDTH) || (y >= DISPLAY_HEIGHT)) return;
    if((x + w - 1) >= DISPLAY_WIDTH) {
      w = DISPLAY_WIDTH - x;
    }
    if((y + h - 1) >= DISPLAY_HEIGHT) {
      h = DISPLAY_HEIGHT - y;
    }


    // Prepare whole line in a single buffer
    color = (color >> 8) | ((color & 0xFF) << 8);
    uint16_t line[DISPLAY_MAX_LINE_PIXELS];
    for(x = 0; x < w; ++x) {
      line[x] = color;
    }

    display_select();
    display_set_addr_window( x, y, w, h );
    // after call display_set_addr_window() display in data mode
    uint32_t line_bytes = w * sizeof(color);
    for( y = h; y > 0; y-- ) {
        display_write( (uint8_t *)line, line_bytes );
    }

    display_deselect();
}


// initialize display, ILI9341, 320x240, spi four lines
void display_init_dma() {
  // enable SP  // configure LCD pins (PA0 - RST, PA3 - DC, PA4 - CS, PA5 - SCK, PA6 - MISO, PA7 - MOSI, PA8 - backlight on/off)
  // PA6 - input with pullup, PA5, PA7 - output push-pull 50 MHz alternate fn
  // PA0, PA3, PA4, PA8 - output push-pull 2MHz
  GPIO_CTL0(GPIOA) = (GPIO_CTL0(GPIOA) & ~(GPIO_MODE_MASK0(0) | GPIO_MODE_MASK0(3) | GPIO_MODE_MASK0(4) | GPIO_MODE_MASK0(5) | GPIO_MODE_MASK0(6)  | GPIO_MODE_MASK0(7)))
                   | GPIO_MODE_SET0(0, 0x0F & (GPIO_MODE_OUT_PP | GPIO_OSPEED_10MHZ))
                   | GPIO_MODE_SET0(3, 0x0F & (GPIO_MODE_OUT_PP | GPIO_OSPEED_10MHZ))
                   | GPIO_MODE_SET0(4, 0x0F & (GPIO_MODE_OUT_PP | GPIO_OSPEED_10MHZ))
                   | GPIO_MODE_SET0(5, 0x0F & (GPIO_MODE_AF_PP | GPIO_OSPEED_50MHZ))
                   | GPIO_MODE_SET0(6, 0x0F & GPIO_MODE_IPU)
                   | GPIO_MODE_SET0(7, 0x0F & (GPIO_MODE_AF_PP | GPIO_OSPEED_50MHZ))
                   ;
  GPIO_CTL1(GPIOA) = (GPIO_CTL1(GPIOA) & ~(GPIO_MODE_MASK1(8)))
                   | GPIO_MODE_SET1(8, 0x0F & (GPIO_MODE_OUT_PP | GPIO_OSPEED_2MHZ))
                   ;
  // set pullups and enable display backlight, display reset passive, deselect display
  GPIO_BOP(GPIOA) = GPIO_BOP_BOP6 | GPIO_BOP_BOP8 | GPIO_BOP_BOP0 | GPIO_BOP_BOP4;
  // enable SPI0 clocks
  RCU_APB2EN |= RCU_APB2EN_SPI0EN;
  // SPI clock = APB2 clock / 8 (64 / 8 = 8 MHz), master mode
  SPI_CTL0(SPI0) = SPI_CTL0_SWNSSEN
                 | SPI_CTL0_SWNSS
                 | SPI_CTL0_SPIEN
                 | SPI_PSC_8
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
  display_reset();
  delay_ms(5);
  display_select();
  // issue init commands set
  for ( const uint8_t * v_ptr = g_ili9341_init; *v_ptr; ) {
    // read data byte count, advance ptr
    uint32_t v_cnt = *v_ptr++;
    // read cmd byte, advance ptr
    uint8_t v_cmd = *v_ptr++;
    // write cmd
    display_write_cmd_dma( v_cmd );
    // write cmd dta
    display_write_data_dma( v_ptr, v_cnt );
    // advance ptr
    v_ptr += v_cnt;
  }
  display_write_cmd_dma( ILI9341_SLEEP_OUT );
  delay_ms(6);
  display_write_cmd_dma( ILI9341_DISPLAY_ON );
  display_deselect();
}


// initialize display, ILI9341, 320x240, serial 4-wire iterface, bitbang
void display_init() {
  // enable SP  // configure LCD pins (PA0 - RST, PA3 - DC, PA4 - CS, PA5 - SCK, PA6 - MISO, PA7 - MOSI, PA8 - backlight on/off)
  // PA6 - input with pullup, PA5, PA7 - output push-pull 50 MHz alternate fn
  // PA0, PA3, PA4, PA8 - output push-pull 2MHz
  GPIO_CTL0(GPIOA) = (GPIO_CTL0(GPIOA) & ~(GPIO_MODE_MASK0(0) | GPIO_MODE_MASK0(3) | GPIO_MODE_MASK0(4) | GPIO_MODE_MASK0(5) | GPIO_MODE_MASK0(6)  | GPIO_MODE_MASK0(7)))
                   | GPIO_MODE_SET0(0, 0x0F & (GPIO_MODE_OUT_PP | GPIO_OSPEED_10MHZ))
                   | GPIO_MODE_SET0(3, 0x0F & (GPIO_MODE_OUT_PP | GPIO_OSPEED_10MHZ))
                   | GPIO_MODE_SET0(4, 0x0F & (GPIO_MODE_OUT_PP | GPIO_OSPEED_10MHZ))
                   | GPIO_MODE_SET0(5, 0x0F & (GPIO_MODE_OUT_PP | GPIO_OSPEED_10MHZ))
                   | GPIO_MODE_SET0(6, 0x0F & GPIO_MODE_IPU)
                   | GPIO_MODE_SET0(7, 0x0F & (GPIO_MODE_OUT_PP | GPIO_OSPEED_10MHZ))
                   ;
  GPIO_CTL1(GPIOA) = (GPIO_CTL1(GPIOA) & ~(GPIO_MODE_MASK1(8)))
                   | GPIO_MODE_SET1(8, 0x0F & (GPIO_MODE_OUT_PP | GPIO_OSPEED_10MHZ))
                   ;
  // set pullups and enable display backlight, display reset passive, deselect display, SCK low
  GPIO_BOP(GPIOA) = GPIO_BOP_BOP6 | GPIO_BOP_BOP8 | GPIO_BOP_BOP0 | GPIO_BOP_BOP4 | GPIO_BOP_CR5;
  // reset sequence
  display_select();
  display_reset();
  delay_ms(150);
  //
  // issue init commands set
  for ( const uint8_t * v_ptr = g_ili9341_init; 0 != *v_ptr; ) {
    // read data byte count, advance ptr
    uint32_t v_cnt = *v_ptr++;
    // read cmd byte, advance ptr
    uint8_t v_cmd = *v_ptr++;
    // write cmd
    display_write_cmd( v_cmd );
    // write cmd dta
    display_write_data( v_ptr, v_cnt );
    // advance ptr
    v_ptr += v_cnt;
  }
  display_write_cmd( ILI9341_SLEEP_OUT );
  delay_ms(6);
  display_write_cmd( ILI9341_DISPLAY_ON );
  //
  display_deselect();
}
