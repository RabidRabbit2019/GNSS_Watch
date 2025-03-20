#include "xpt2046.h"
#include "n200_func.h"
#include "gd32vf103.h"

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


#define XPT2046_READ_Z1     0xB1
#define XPT2046_READ_Z2     0xC1
#define XPT2046_READ_X      0xD1
#define XPT2046_READ_Y      0x91
#define XPT2046_READ_Y_0    0xD0


#ifdef __cplusplus
extern "C" {
#endif

void delay_ms( uint32_t a_ms );
extern volatile uint32_t g_milliseconds;

volatile bool g_xpt2046_touch_flag = false;
static int Ax = 0, Bx = 0, Dx = 0, Ay = 0, By = 0, Dy = 0;


// проверяем состояние экрана - есть ли нажатие
bool xpt2046_touched() {
  return 0 == (GPIO_ISTAT(GPIOB) & BIT(XPT2046_PIN_INT));
}


// ждать "отпускания" экрана
void xpt2046_wait_release( unsigned int a_timeout_ms ) {
  for ( uint32_t v_from = g_milliseconds; (g_milliseconds - v_from) < a_timeout_ms; ) {
    if ( 0 != (GPIO_ISTAT(GPIOB) & BIT(XPT2046_PIN_INT)) ) {
      return;
    }
  }
}


// выдача команды (8 битов) и получение результата (16 битов)
static int xpt2046_spi_xfer16( uint8_t a_byte ) {
  SPI_DATA(SPI1);
  // три байта на отправку
  // первый байт
  SPI_DATA(SPI1) = a_byte;
  // принятый байт откидываем
  while ( 0 == (SPI_STAT(SPI1) & SPI_STAT_RBNE) ) {}
  SPI_DATA(SPI1);
  // второй байт
  SPI_DATA(SPI1) = (uint8_t)0;
  // старший байт результата
  while ( 0 == (SPI_STAT(SPI1) & SPI_STAT_RBNE) ) {}
  int v_result = SPI_DATA(SPI1) << 8;
  // третий байт
  SPI_DATA(SPI1) = (uint8_t)0;
  // младший байт
  while ( 0 == (SPI_STAT(SPI1) & SPI_STAT_RBNE) ) {}
  v_result |= SPI_DATA(SPI1);
  // в результате 12 битов, ставим на место и прикладываем маску (на всякий случай)
  return (v_result >> 3) & 0xFFF;
}


// выбрать контроллер на шине
static void xpt2046_select() {
 GPIO_BOP(GPIOB) = GPIO_BOP_RST(XPT2046_PIN_CS);
}


// "отпустить" контроллер
static void xpt2046_release() {
 GPIO_BOP(GPIOB) = GPIO_BOP_SET(XPT2046_PIN_CS);
}


// настройка выводов контроллера и SPI1 для связи с xpt2046
void xpt2046_init() {
  // включение GPIOB и SPI1
  RCU_APB1EN |= RCU_APB1EN_SPI1EN;
  RCU_APB2EN |= RCU_APB2EN_PBEN;
  // настройка выводов
  // PB13..PB15 - push-pull 10MHz alternate fn (SPI1)
  // PB12 - выход push-pull
  // PB11 - вход
  GPIO_CTL1(GPIOB) = (GPIO_CTL1(GPIOB) & ~(GPIO_MODE_MASK1(XPT2046_PIN_CS) | GPIO_MODE_MASK1(XPT2046_PIN_INT) | GPIO_MODE_MASK1(XPT2046_PIN_MISO) | GPIO_MODE_MASK1(XPT2046_PIN_MOSI) | GPIO_MODE_MASK1(XPT2046_PIN_SCK)))
                   | GPIO_MODE_SET1(XPT2046_PIN_MISO, 0x0F & (GPIO_MODE_AF_PP | GPIO_OSPEED_10MHZ))
                   | GPIO_MODE_SET1(XPT2046_PIN_MOSI, 0x0F & (GPIO_MODE_AF_PP | GPIO_OSPEED_10MHZ))
                   | GPIO_MODE_SET1(XPT2046_PIN_SCK, 0x0F & (GPIO_MODE_AF_PP | GPIO_OSPEED_10MHZ))
                   | GPIO_MODE_SET1(XPT2046_PIN_CS, 0x0F & GPIO_MODE_OUT_PP)
                   | GPIO_MODE_SET1(XPT2046_PIN_INT, 0x0F & GPIO_MODE_IPU)
                   ;
  // выбор XPT2046 на шине, выход, в лог. 1 (не выбран)
  GPIO_BOP(GPIOB) = GPIO_BOP_SET(XPT2046_PIN_CS);
  // настройка SPI1
  // SPI clock = APB1 clock / 16 (32 / 16 = 2 MHz), master mode
  SPI_CTL0(SPI1) = SPI_CTL0_SWNSSEN
                 | SPI_CTL0_SWNSS
                 | SPI_CTL0_SPIEN
                 | SPI_PSC_16
                 | SPI_CTL0_MSTMOD
                 ;
  // запускаем режим выдачи сигнала "нажатие"
  xpt2046_select();
  delay_ms( 2u );
  xpt2046_spi_xfer16( XPT2046_READ_Y_0 ); // PD1 = 0, PD0 = 0, см. даташит на XPT2046
  delay_ms( 2u );
  xpt2046_release();
  delay_ms( 2u );
  // без задержек не работает
}


// чтение данных о нажатии
bool xpt2046_read( int * a_x, int * a_y, int * a_z )
{
  bool v_result = false;
  // накопительные значения
  int vx = 0, vy = 0, vz1 = 0, vz2 = 0;
  // выбор контроллера на шине SPI_0
  xpt2046_select();
  // задержка
  delay_ms( 2u );
  // по восемь измерений каждого параметра
  for ( int i = 0; i < XPT2046_AVG_COUNT; ++i ) {
    // первый замер каждого параметра откидываем
    xpt2046_spi_xfer16( XPT2046_READ_X );
    vx += xpt2046_spi_xfer16( XPT2046_READ_X );
    xpt2046_spi_xfer16( XPT2046_READ_Y );
    vy += xpt2046_spi_xfer16( XPT2046_READ_Y );
    xpt2046_spi_xfer16( XPT2046_READ_Z1 );
    vz1 += xpt2046_spi_xfer16( XPT2046_READ_Z1 );
    xpt2046_spi_xfer16( XPT2046_READ_Z2 );
    vz2 += xpt2046_spi_xfer16( XPT2046_READ_Z2 );
  }
  // измерение с переходом в режим ожидания
  xpt2046_spi_xfer16( XPT2046_READ_Y_0 );
  // ждём
  delay_ms( 2u );
  // отпускаем контроллер
  xpt2046_release();
  // ещё ждём
  delay_ms( 2u );
  
  // усреднение
  vx /= XPT2046_AVG_COUNT;
  vy /= XPT2046_AVG_COUNT;
  vz1 /= XPT2046_AVG_COUNT;
  vz2 /= XPT2046_AVG_COUNT;
	
  // если Z1 == 0, то нажатия точно нет
  if ( 0 != vz1 ) {
    // расчитываем сопротивление от нажатия (см. даташит на XPT2046)
    *a_z = (int)((((524288LL * vx * vz2) / vz1) - (524288LL * vx)) / (1024 * 4096));
    // если сопротивление меньше порога, считаем, что есть нажатие
    if ( *a_z < XPT2046_TOUCH_THRESHOLD ) {
      // результат
      *a_x = vx;
      *a_y = vy;
      v_result = true;
    }
  }
  //
  return v_result;
}


// установить коэффициенты расчёта координат касаний экрана
// идея подсмотрена в https://embedded.icu/article/mikrokontrollery/rabota-s-rezistivnym-sensornym-ekranom
// коеффициенты вычисляются при калибровке экрана по четырём касаниям и затем
// используются для получения координат касания в экранной системе координат
// X = Xadc * Ax + Yadc * Bx + Dx;
// Y = Xadc * Ay + Yadc * By + Dy;
void xpt2046_set_coeff( int a_Ax, int a_Bx, int a_Dx, int a_Ay, int a_By, int a_Dy ) {
  Ax = a_Ax;
  Bx = a_Bx;
  Dx = a_Dx;
  Ay = a_Ay;
  By = a_By;
  Dy = a_Dy;
}


// получить X координату на экране по данным от контроллера сенсорной поверхности
int xpt2046_get_X( int a_x, int a_y ) {
  return (a_x * Ax + a_y * Bx + Dx) / 4096;
}


// получить Y координату на экране по данным от контроллера сенсорной поверхности
int xpt2046_get_Y( int a_x, int a_y ) {
  return (a_x * Ay + a_y * By + Dy) / 4096;
}


#ifdef __cplusplus
}
#endif
