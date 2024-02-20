#include "display.h"
#include "gnss.h"
#include "render.h"
#include "rtc.h"
#include "bme280.h"

#include "n200_func.h"
#include "gd32vf103.h"
#include <flag2x2.h>
#include <time.h>
#include <string.h>
#include <stdio.h>


void delay_ms( uint32_t a_ms );


void main() {
  
  // включаем тактирование для PIOA, PIOB, PIOC, AF, SPI0, USART0
  RCU_APB2EN |= ( RCU_APB2EN_PCEN
                | RCU_APB2EN_PBEN
                | RCU_APB2EN_PAEN
                | RCU_APB2EN_AFEN
                | RCU_APB2EN_SPI0EN
                | RCU_APB2EN_USART0EN
                );
  // включаем тактирование DMA0
  RCU_AHBEN |= RCU_AHBEN_DMA0EN;
  // управление RGB-светодиодом
  // PC13 Push-Pull 2MHz output
  GPIO_CTL1(GPIOC) = (GPIO_CTL1(GPIOC) & ~GPIO_MODE_MASK1(13))
                   | GPIO_MODE_SET1(13, 0x0F & (GPIO_MODE_OUT_PP | GPIO_OSPEED_2MHZ))
                   ;
  // PA1, PA2 Push-Pull 2MHz output
  GPIO_CTL0(GPIOA) = (GPIO_CTL0(GPIOA) & ~(GPIO_MODE_MASK0(1) | GPIO_MODE_MASK0(2)))
                   | GPIO_MODE_SET0(1, 0x0F & (GPIO_MODE_OUT_PP | GPIO_OSPEED_2MHZ))
                   | GPIO_MODE_SET0(2, 0x0F & (GPIO_MODE_OUT_PP | GPIO_OSPEED_2MHZ))
                   ;
  // RGB-светодиод выключаем
  // 1 на PC13 (красный выключен)
  GPIO_BOP(GPIOC) |= GPIO_BOP_BOP13;
  // 1 на PA1, PA2 (зелёный и синий выключены)
  GPIO_BOP(GPIOA) |= (GPIO_BOP_BOP1 | GPIO_BOP_BOP2);
  
  // переносим функции I2C0 (PB9 - SDA, PB8 - SCL)
  AFIO_PCF0 |= AFIO_PCF0_I2C0_REMAP;
  // настройка I2C выводов и входов под кнопки
  // PB8 и PB9 - выходы open-drain 2MHz alternate fn
  // PB13..PB15 - входы с подтяжкой к питанию
  GPIO_CTL1(GPIOB) = (GPIO_CTL1(GPIOB) & ~(GPIO_MODE_MASK1(9) | GPIO_MODE_MASK1(8) | GPIO_MODE_MASK1(13) | GPIO_MODE_MASK1(14) | GPIO_MODE_MASK1(15)))
                   | GPIO_MODE_SET1(9, 0x0F & (GPIO_MODE_AF_OD | GPIO_OSPEED_2MHZ))
                   | GPIO_MODE_SET1(8, 0x0F & (GPIO_MODE_AF_OD | GPIO_OSPEED_2MHZ))
                   | GPIO_MODE_SET1(15, 0x0F & GPIO_MODE_IPU)
                   | GPIO_MODE_SET1(14, 0x0F & GPIO_MODE_IPU)
                   | GPIO_MODE_SET1(13, 0x0F & GPIO_MODE_IPU)
                   ;
  // включаем подтяжку к питанию для входов кнопок
  GPIO_BOP(GPIOB) = GPIO_BOP_BOP13 | GPIO_BOP_BOP14 | GPIO_BOP_BOP15;
  /*
  // init EXTI14 interrupt (PB14 for GNSS PPS)
  // connect PB14 as interrupt source
  AFIO_EXTISS3 = GPIO_PORT_SOURCE_GPIOB << 8;
  // enable EXTI14 rising edge detector
  EXTI_FTEN = EXTI_FTEN_FTEN14;
  // clear pending EXTI14 interrupt
  EXTI_PD = EXTI_PD_PD14;
  // enable EXTI14 interrupt
  EXTI_INTEN = EXTI_INTEN_INTEN14;
  */
  // настройка экрана
  display_init_dma();
  //
  init_BMP280();
  init_RTC();
  init_GNSS();
  init_RENDER();
  // test display
  display_draw_jpeg_image( 0, 0, flag2x2_jpg, flag2x2_jpg_len );
  delay_ms( 5000 );
  // clear display with black
  display_fill_rectangle_dma( 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_COLOR_BLACK );
  //
  uint32_t v_ts = g_milliseconds;
  // loop
  for (;;) {
    uint32_t v_diff = (uint32_t)(g_milliseconds - v_ts);
    if ( v_diff > 1000u ) {
      ++g_time;
      v_ts += 1000u;
    }
    //
    time_slice_GNSS();
    time_slice_RENDER();
  }
}
