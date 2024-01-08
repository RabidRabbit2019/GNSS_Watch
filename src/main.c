#include "display.h"

#include "n200_func.h"
#include "gd32vf103.h"
#include <flag_320_240.h>
#include <font_22_24.h>
#include <font_110_110.h>
#include <time.h>
#include <string.h>


void delay_ms( uint32_t a_ms );

void main() {
  
  // enable clocks for PIOA, PIOB and PIOC
  RCU_APB2EN |= (RCU_APB2EN_PCEN | RCU_APB2EN_PBEN | RCU_APB2EN_PAEN);
  // PC13 Push-Pull 2MHz output
  GPIO_CTL1(GPIOC) = (GPIO_CTL1(GPIOC) & ~GPIO_MODE_MASK1(13))
                   | GPIO_MODE_SET1(13, 0x0F & (GPIO_MODE_OUT_PP | GPIO_OSPEED_2MHZ))
                   ;
  // PA1, PA2 Push-Pull 2MHz output
  GPIO_CTL0(GPIOA) = (GPIO_CTL0(GPIOA) & ~(GPIO_MODE_MASK0(1) | GPIO_MODE_MASK0(2)))
                   | GPIO_MODE_SET0(1, 0x0F & (GPIO_MODE_OUT_PP | GPIO_OSPEED_2MHZ))
                   | GPIO_MODE_SET0(2, 0x0F & (GPIO_MODE_OUT_PP | GPIO_OSPEED_2MHZ))
                   ;
  // RGB LED off
  // set 1 for PC13 (R led OFF)
  GPIO_BOP(GPIOC) |= GPIO_BOP_BOP13;
  // set 1 for PA1, PA2 (G and B leds OFF)
  GPIO_BOP(GPIOA) |= (GPIO_BOP_BOP1 | GPIO_BOP_BOP2);
  
  // remap I2C0 (PB9 - SDA, PB8 - SCL) and USART0 (PB7 - RX, PB6 - TX)
  AFIO_PCF0 |= (AFIO_PCF0_I2C0_REMAP | AFIO_PCF0_USART0_REMAP);
  // configure I2C pins and inputs for buttons
  // PB8 and PB9 - output open-drain 2MHz alternate fn
  // PB13..PB15 - input with pullups
  GPIO_CTL1(GPIOB) = (GPIO_CTL1(GPIOB) & ~(GPIO_MODE_MASK1(9) | GPIO_MODE_MASK1(8) | GPIO_MODE_MASK1(13) | GPIO_MODE_MASK1(14) | GPIO_MODE_MASK1(15)))
                   | GPIO_MODE_SET1(9, 0x0F & (GPIO_MODE_AF_OD | GPIO_OSPEED_2MHZ))
                   | GPIO_MODE_SET1(8, 0x0F & (GPIO_MODE_AF_OD | GPIO_OSPEED_2MHZ))
                   | GPIO_MODE_SET1(15, 0x0F & GPIO_MODE_IPU)
                   | GPIO_MODE_SET1(14, 0x0F & GPIO_MODE_IPU)
                   | GPIO_MODE_SET1(13, 0x0F & GPIO_MODE_IPU)
                   ;
  // configure USART pins
  // PB7 input with pullup, PB6 output push-pull 2MHz alternate fn
  GPIO_CTL0(GPIOB) = (GPIO_CTL0(GPIOB) & ~(GPIO_MODE_MASK0(7) | GPIO_MODE_MASK0(6)))
                   | GPIO_MODE_SET0(7, 0x0F & GPIO_MODE_IPU)
                   | GPIO_MODE_SET0(6, 0x0F & (GPIO_MODE_AF_PP | GPIO_OSPEED_2MHZ))
                   ;
  // set pullups
  GPIO_BOP(GPIOB) = GPIO_BOP_BOP7 | GPIO_BOP_BOP13 | GPIO_BOP_BOP14 | GPIO_BOP_BOP15;
  // init display
  display_init_dma();
  // test display
  //display_draw_zic_image( 0, 0, Iflag_320_240_tga_width, Iflag_320_240_tga_height, Iflag_320_240_tga_zic, sizeof(Iflag_320_240_tga_zic) );
  //
  //delay_ms( 2000 );
  //
  display_fill_rectangle_dma( 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_COLOR_BLACK );
  //
  char v_time_str[6];
  v_time_str[2] = ':';
  v_time_str[5] = '\0';
  //
  time_t v_time = 1700000000;
  uint32_t v_ts = g_milliseconds;
  struct tm v_tm;
  //
  display_write_string( 0, 140, "board: Longan Nano", &font_22_24_font, DISPLAY_COLOR_YELLOW, DISPLAY_COLOR_BLACK );
  display_write_string( 0, 164, "mcu: GD32VF103 (riscv32)", &font_22_24_font, DISPLAY_COLOR_YELLOW, DISPLAY_COLOR_BLACK );
  // loop
  for (;;) {
    //
    uint32_t v_now = g_milliseconds;
    uint32_t v_diff = (uint32_t)(v_now - v_ts);
    if ( v_diff > 1000u ) {
      ++v_time;
      v_ts += 1000u;
    }
    gmtime_r( &v_time, &v_tm );
    v_time_str[0] = (v_tm.tm_hour / 10) + '0';
    v_time_str[1] = (v_tm.tm_hour % 10) + '0';
    v_time_str[3] = (v_tm.tm_min / 10) + '0';
    v_time_str[4] = (v_tm.tm_min % 10) + '0';
    diplay_write_string_with_background(
        0
      , 0
      , 320
      , font_110_110_font.m_row_height
      , v_time_str
      , &font_110_110_font
      , DISPLAY_COLOR_WHITE
      , DISPLAY_COLOR_DARKBLUE
      , DISPLAY_COLOR_DARKGREEN
      , v_tm.tm_sec
      );
    // set 0 for PC13 (R led ON)
    GPIO_BOP(GPIOC) |= GPIO_BOP_CR13;
    delay_ms(25);
    // set 1 for PC13 (R led OFF)
    GPIO_BOP(GPIOC) |= GPIO_BOP_BOP13;
    // set 0 for PA1 (G led ON)
    GPIO_BOP(GPIOA) |= GPIO_BOP_CR1;
    delay_ms(25);
    // set 1 for PA1 (G led OFF)
    GPIO_BOP(GPIOA) |= GPIO_BOP_BOP1;
    // set 0 for PA2 (B led ON)
    GPIO_BOP(GPIOA) |= GPIO_BOP_CR2;
    delay_ms(25);
    // set 1 for PA2 (B led OFF)
    GPIO_BOP(GPIOA) |= GPIO_BOP_BOP2;
  }
}
