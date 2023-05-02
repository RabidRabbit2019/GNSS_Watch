#include "n200_func.h"
#include "gd32vf103.h"


void delay_ms( uint32_t a_ms );

void main() {
  
  // enable clocks for PIOA, PIOB and PIOC
  RCU_APB2EN |= (RCU_APB2EN_PCEN | RCU_APB2EN_PBEN | RCU_APB2EN_PAEN);
  // PC13 Push-Pull 2MHz output
  GPIO_CTL1(GPIOC) = (GPIO_CTL1(GPIOC) & ~GPIO_MODE_MASK(13-8))
                   | GPIO_MODE_SET(13 - 8, 0x0F & (GPIO_MODE_OUT_PP | GPIO_OSPEED_2MHZ))
                   ;
  // PA1, PA2 Push-Pull 2MHz output
  GPIO_CTL0(GPIOA) = (GPIO_CTL0(GPIOA) & ~(GPIO_MODE_MASK(1) | GPIO_MODE_MASK(2)))
                   | GPIO_MODE_SET(1, 0x0F & (GPIO_MODE_OUT_PP | GPIO_OSPEED_2MHZ))
                   | GPIO_MODE_SET(2, 0x0F & (GPIO_MODE_OUT_PP | GPIO_OSPEED_2MHZ))
                   ;
  // RGB LED off
  // set 1 for PC13 (R led OFF)
  GPIO_BOP(GPIOC) |= GPIO_BOP_BOP13;
  // set 1 for PA1, PA2 (G and B leds OFF)
  GPIO_BOP(GPIOA) |= (GPIO_BOP_BOP1 | GPIO_BOP_BOP2);
  
  // remap I2C0 (PB9, PB8) and USART0 (PB7, PB6)
  AFIO_PCF0 |= (AFIO_PCF0_I2C0_REMAP | AFIO_PCF0_USART0_REMAP);
  // configure I2C pins
  // PB8 and PB9 - output open-drain 2MHz alternate fn
  GPIO_CTL1(GPIOB) = (GPIO_CTL1(GPIOB) & ~(GPIO_MODE_MASK(9-8) | GPIO_MODE_MASK(8-8)))
                   | GPIO_MODE_SET(9-8, 0x0F & (GPIO_MODE_AF_OD | GPIO_OSPEED_2MHZ))
                   | GPIO_MODE_SET(8-8, 0x0F & (GPIO_MODE_AF_OD | GPIO_OSPEED_2MHZ))
                   ;
  // configure USART pins
  // PB7 input, PB6 output push-pull 2MHz alternate fn
  GPIO_CTL0(GPIOB) = (GPIO_CTL0(GPIOB) & ~(GPIO_MODE_MASK(7) | GPIO_MODE_MASK(6)))
                   | GPIO_MODE_SET(7, 0x0F & GPIO_MODE_IN_FLOATING)
                   | GPIO_MODE_SET(6, 0x0F & (GPIO_MODE_AF_PP | GPIO_OSPEED_2MHZ))
                   ;
  // configure SPI pins
                   
  
  
  // loop
  for (;;) {
    delay_ms(250);
    // set 0 for PC13 (R led ON)
    GPIO_BOP(GPIOC) |= GPIO_BOP_CR13;
    delay_ms(250);
    // set 1 for PC13 (R led OFF)
    GPIO_BOP(GPIOC) |= GPIO_BOP_BOP13;
    delay_ms(250);
    // set 0 for PA1 (G led ON)
    GPIO_BOP(GPIOA) |= GPIO_BOP_CR1;
    delay_ms(250);
    // set 1 for PA1 (G led OFF)
    GPIO_BOP(GPIOA) |= GPIO_BOP_BOP1;
    delay_ms(250);
    // set 0 for PA2 (B led ON)
    GPIO_BOP(GPIOA) |= GPIO_BOP_CR2;
    delay_ms(250);
    // set 1 for PA2 (B led OFF)
    GPIO_BOP(GPIOA) |= GPIO_BOP_BOP2;
  }
}
