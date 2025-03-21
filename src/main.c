#include "display.h"
#include "gnss.h"
#include "render.h"
#include "rtc.h"
#include "bme280.h"
#include "xpt2046.h"
#include "settings.h"
#include "n200_func.h"
#include "gd32vf103.h"
#include <flag2x2.h>
#include <font_28_32.h>

#include <time.h>
#include <string.h>
#include <stdio.h>


void delay_ms( uint32_t a_ms );
void calibrate_touchscreen();

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
  GPIO_BOP(GPIOC) = GPIO_BOP_BOP13;
  // 1 на PA1, PA2 (зелёный и синий выключены)
  GPIO_BOP(GPIOA) = (GPIO_BOP_BOP1 | GPIO_BOP_BOP2);
  
  // переносим функции I2C0 (PB9 - SDA, PB8 - SCL)
  AFIO_PCF0 |= AFIO_PCF0_I2C0_REMAP;
  // настройка I2C выводов и входов под кнопки
  // PB8 и PB9 - open-drain 2MHz alternate fn (I2C0)
  GPIO_CTL1(GPIOB) = (GPIO_CTL1(GPIOB) & ~(GPIO_MODE_MASK1(9) | GPIO_MODE_MASK1(8)))
                   | GPIO_MODE_SET1(8, 0x0F & (GPIO_MODE_AF_OD | GPIO_OSPEED_2MHZ))
                   | GPIO_MODE_SET1(9, 0x0F & (GPIO_MODE_AF_OD | GPIO_OSPEED_2MHZ))
                   ;
  // инициализация хранилища настроек
  settings_init();
  // настройка экрана
  display_init_dma();
  xpt2046_init();
  //
  init_BMP280();
  init_RTC();
  init_GNSS();
  init_RENDER();
  //
  calibrate_touchscreen();
  // test display
  //display_draw_jpeg_image( 0, 0, flag2x2_jpg, flag2x2_jpg_len );
  //delay_ms( 5000 );
  // clear display with black
  //display_fill_rectangle_dma( 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_COLOR_BLACK );
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


// калибровка тачскрина с записью полученных коэффициентов в последнюю страницу флэша
// так что доступный размер флэша для программы на 1 страницу меньше :)
void calibrate_touchscreen() {
  touch_coeff_s v_coeff;
  // если есть нажатие на экран или не удалось прочитать коэфиициенты из флэша
  if ( xpt2046_touched() || !load_coeff( &v_coeff ) ) {
    display_fill_rectangle_dma( 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_COLOR_BLACK );
    if ( xpt2046_touched() ) {
      int v_y = (DISPLAY_HEIGHT - (font_28_32_font.m_row_height * 3)) / 2;
      diplay_write_string_with_background(
            0, v_y
          , DISPLAY_WIDTH, font_28_32_font.m_row_height
          , "для начала"
          , &font_28_32_font
          , DISPLAY_COLOR_GREEN
          , DISPLAY_COLOR_BLACK
          , DISPLAY_COLOR_BLACK
          , 0
          );
      v_y += font_28_32_font.m_row_height;
      diplay_write_string_with_background(
            0, v_y
          , DISPLAY_WIDTH, font_28_32_font.m_row_height
          , "калибровки"
          , &font_28_32_font
          , DISPLAY_COLOR_GREEN
          , DISPLAY_COLOR_BLACK
          , DISPLAY_COLOR_BLACK
          , 0
          );
      v_y += font_28_32_font.m_row_height;
      diplay_write_string_with_background(
            0, v_y
          , DISPLAY_WIDTH, font_28_32_font.m_row_height
          , "отпустите экран"
          , &font_28_32_font
          , DISPLAY_COLOR_GREEN
          , DISPLAY_COLOR_BLACK
          , DISPLAY_COLOR_BLACK
          , 0
          );
      while( xpt2046_touched() ) {}
      display_fill_rectangle_dma( 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_COLOR_BLACK );
      delay_ms( 250 );
    }
    // калибровка
    // идея подсмотрена в https://embedded.icu/article/mikrokontrollery/rabota-s-rezistivnym-sensornym-ekranom
    // после калибровки координаты касаний будут соответствовать координатам в системе координат экрана
    // с учётом поворота осей и зеркалирования
    int v_y = (DISPLAY_HEIGHT - (font_28_32_font.m_row_height * 3)) / 2;
    diplay_write_string_with_background(
          0, v_y
        , DISPLAY_WIDTH, font_28_32_font.m_row_height
        , "-калибровка-"
        , &font_28_32_font
        , DISPLAY_COLOR_YELLOW
        , DISPLAY_COLOR_DARKBLUE
        , DISPLAY_COLOR_DARKBLUE
        , 0
        );
    diplay_write_string_with_background(
          0, v_y + font_28_32_font.m_row_height
        , DISPLAY_WIDTH, font_28_32_font.m_row_height
        , "касайтесь экрана"
        , &font_28_32_font
        , DISPLAY_COLOR_WHITE
        , DISPLAY_COLOR_DARKGRAY
        , DISPLAY_COLOR_DARKGRAY
        , 0
        );
    diplay_write_string_with_background(
          0, v_y + font_28_32_font.m_row_height * 2
        , DISPLAY_WIDTH, font_28_32_font.m_row_height
        , "там, где крестик"
        , &font_28_32_font
        , DISPLAY_COLOR_WHITE
        , DISPLAY_COLOR_DARKGRAY
        , DISPLAY_COLOR_DARKGRAY
        , 0
        );
    //
    int z;
    int corners[4][2];
    // левый верхний угол
    display_fill_rectangle_dma( 8, 0, 1, 17, DISPLAY_COLOR_BLUE );
    display_fill_rectangle_dma( 0, 8, 17, 1, DISPLAY_COLOR_BLUE );
    // ждём нажатия
    for (;;) {
      while ( !xpt2046_touched() ) {}
      if ( xpt2046_read( &corners[0][0], &corners[0][1], &z ) ) {
        break;
      }
    }
    display_fill_rectangle_dma( 0, 0, 17, 17, DISPLAY_COLOR_BLACK );
    //ждём отпускания экрана
    while ( xpt2046_touched() ) {}
    delay_ms( 250 );
    // правый верхний угол
    display_fill_rectangle_dma( DISPLAY_WIDTH - 9, 0, 1, 17, DISPLAY_COLOR_BLUE );
    display_fill_rectangle_dma( DISPLAY_WIDTH - 17, 8, 17, 1, DISPLAY_COLOR_BLUE );
    // ждём нажатия
    for (;;) {
      while ( !xpt2046_touched() ) {}
      if ( xpt2046_read( &corners[1][0], &corners[1][1], &z ) ) {
        break;
      }
    }
    display_fill_rectangle_dma( DISPLAY_WIDTH - 17, 0, 17, 17, DISPLAY_COLOR_BLACK );
    //ждём отпускания экрана
    while ( xpt2046_touched() ) {}
    delay_ms( 250 );
    // левый нижний угол
    display_fill_rectangle_dma( 8, DISPLAY_HEIGHT - 17, 1, 17, DISPLAY_COLOR_BLUE );
    display_fill_rectangle_dma( 0, DISPLAY_HEIGHT - 9, 17, 1, DISPLAY_COLOR_BLUE );
    // ждём нажатия
    for (;;) {
      while ( !xpt2046_touched() ) {}
      if ( xpt2046_read( &corners[2][0], &corners[2][1], &z ) ) {
        break;
      }
    }
    display_fill_rectangle_dma( 0, DISPLAY_HEIGHT - 17, 17, 17, DISPLAY_COLOR_BLACK );
    //ждём отпускания экрана
    while ( xpt2046_touched() ) {}
    delay_ms( 250 );
    // правый нижний угол
    display_fill_rectangle_dma( DISPLAY_WIDTH - 9, DISPLAY_HEIGHT - 17, 1, 17, DISPLAY_COLOR_BLUE );
    display_fill_rectangle_dma( DISPLAY_WIDTH - 17, DISPLAY_HEIGHT - 9, 17, 1, DISPLAY_COLOR_BLUE );
    // ждём нажатия
    for (;;) {
      while ( !xpt2046_touched() ) {}
      if ( xpt2046_read( &corners[3][0], &corners[3][1], &z ) ) {
        break;
      }
    }
    display_fill_rectangle_dma( DISPLAY_WIDTH - 17, DISPLAY_HEIGHT - 17, 17, 17, DISPLAY_COLOR_BLACK );
    //ждём отпускания экрана
    while ( xpt2046_touched() ) {}
    delay_ms( 250 );
    // определяем коэффициенты
    int Ax = 0, Bx = 0, Dx = 0, Ay = 0, By = 0, Dy = 0;
    // определяем совпадение осей координат
    // смотрим на разницу по Y двух точек с одинаковыми координатами по Y в экранной системе координат
    int dd = corners[1][1] - corners[0][1];
    if ( dd < 0 ) {
      dd = 0 - dd;
    }
    //
    do {
      // число 1024 подходит для 12-битных отсчётов от контроллера сенсорной поверхности
      if ( dd < 1024 ) {
        // коэффициенты Ay = 0 и Bx = 0, т.е. оси экрана (X, Y) совпадают с осями сенсорной поверхности
        // различия замеров по координатам
        int dx1 = corners[1][0] - corners[0][0];
        int dx2 = corners[3][0] - corners[2][0];
        int dy1 = corners[2][1] - corners[0][1];
        int dy2 = corners[3][1] - corners[1][1];
        if ( 0 == dx1 || 0 == dx2 || 0 == dy1 || 0 == dy2 ) {
          break;
        }
        // расчитываем коэффициенты
        Ax = ( ((4096 * (DISPLAY_WIDTH - 8 - 8)) / dx1) + ((4096 * (DISPLAY_WIDTH - 8 - 8)) / dx2) ) / 2;
        Dx = ((4096 * 8) - (corners[0][0] * Ax))
           + ((4096 * (DISPLAY_WIDTH - 8)) - (corners[1][0] * Ax))
           + ((4096 * 8) - (corners[2][0] * Ax))
           + ((4096 * (DISPLAY_WIDTH - 8)) - (corners[3][0] * Ax))
           ;
        Dx /= 4;
        By = ( ((4096 * (DISPLAY_HEIGHT - 8 - 8)) / dy1) + ((4096 * (DISPLAY_HEIGHT - 8 - 8)) / dy2) ) / 2;
        Dy = ((4096 * 8) - (corners[0][1] * By))
           + ((4096 * (DISPLAY_HEIGHT - 8)) - (corners[2][1] * By))
           + ((4096 * 8) - (corners[1][1] * By))
           + ((4096 * (DISPLAY_HEIGHT - 8)) - (corners[3][1] * By))
           ;
        Dy /= 4;
      } else {
        // коэффициенты Ax = 0 и By = 0, т.е. оси экрана (X, Y) не совпадают с осями сенсорной поверхности
        // (X экрана соответствует Y сенсорной поверхности, Y экрана - X сенсорной поверхности)
        // различия замеров по координатам
        int dx1 = corners[1][1] - corners[0][1];
        int dx2 = corners[3][1] - corners[2][1];
        int dy1 = corners[2][0] - corners[0][0];
        int dy2 = corners[3][0] - corners[1][0];
        if ( 0 == dx1 || 0 == dx2 || 0 == dy1 || 0 == dy2 ) {
          break;
        }
        // расчитываем коэффициенты
        Bx = ( ((4096 * (DISPLAY_WIDTH - 8 - 8)) / dx1) + ((4096 * (DISPLAY_WIDTH - 8 - 8)) / dx2) ) / 2;
        Dx = ((4096 * 8) - (corners[0][0] * Bx))
           + ((4096 * (DISPLAY_WIDTH - 8)) - (corners[1][0] * Bx))
           + ((4096 * 8) - (corners[2][0] * Bx))
           + ((4096 * (DISPLAY_WIDTH - 8)) - (corners[3][0] * Bx))
           ;
        Dx /= 4;
        // printf( "Bx = %d, Dx = %d\n", Bx, Dx );
        Ay = ( ((4096 * (DISPLAY_HEIGHT - 8 - 8)) / dy1) + ((4096 * (DISPLAY_HEIGHT - 8 - 8)) / dy2) ) / 2;
        Dy = ((4096 * 8) - (corners[0][1] * Ay))
           + ((4096 * (DISPLAY_HEIGHT - 8)) - (corners[2][1] * Ay))
           + ((4096 * 8) - (corners[1][1] * Ay))
           + ((4096 * (DISPLAY_HEIGHT - 8)) - (corners[3][1] * Ay))
           ;
        Dy /= 4;
      }
    } while (false);
    // записываем коэффициенты
    v_coeff.Ax = Ax;
    v_coeff.Bx = Bx;
    v_coeff.Dx = Dx;
    v_coeff.Ay = Ay;
    v_coeff.By = By;
    v_coeff.Dy = Dy;
    store_coeff( &v_coeff );
  }
  // устанавливаем коэффициенты
  xpt2046_set_coeff( v_coeff.Ax, v_coeff.Bx, v_coeff.Dx, v_coeff.Ay, v_coeff.By, v_coeff.Dy );
  //
  display_fill_rectangle_dma( 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_COLOR_BLACK );
}
