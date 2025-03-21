#include "render.h"
#include "gnss.h"
#include "display.h"
#include "rtc.h"
#include "bme280.h"
#include <font_28_32.h>
#include <font_110_110.h>
#include <gnss_ok.h>
#include <gnss_no.h>
#include <alarm_clock.h>
#include <settings.h>

#include <time.h>
#include <stdio.h>


#define RENDER_MODE_CLOCK     0
#define RENDER_MODE_ALARM     1
#define RENDER_MODE_SETTINGS  2

static const char * g_lang_names[3] = {
  "RUS", "ENG", "FRA"
};

static const char * g_month_names[3][12] = {
  {
    "Января"
  , "Февраля"
  , "Марта"
  , "Апреля"
  , "Мая"
  , "Июня"
  , "Июля"
  , "Августа"
  , "Сентября"
  , "Октября"
  , "Ноября"
  , "Декабря"
  }
, {
    "January"
  , "February"
  , "March"
  , "April"
  , "May"
  , "June"
  , "July"
  , "August"
  , "September"
  , "October"
  , "November"
  , "December"
}
, {
    "Janvier"
  , "Février"
  , "Mars"
  , "Avril"
  , "Mai"
  , "Juin"
  , "Juillet"
  , "Août"
  , "Septembre"
  , "Octobre"
  , "Novembre"
  , "Décembre"
}
};


static const uint16_t g_bgcolors[] = {
  DISPLAY_COLOR_DARKGREEN
, DISPLAY_COLOR_DARKCYAN
, DISPLAY_COLOR_DARKBLUE
, DISPLAY_COLOR_DARKCYAN
};

// render screen mode
static int g_render_mode = RENDER_MODE_CLOCK;
static int g_rended_mode_last = RENDER_MODE_CLOCK;
static int g_bgcolor_index = 0;
static int g_last_tm_min = 0;
static int g_lang_index = 0;
static uint32_t g_bgcolors_current = DISPLAY_COLOR_DARKCYAN | (DISPLAY_COLOR_DARKGREEN << 16);


static uint32_t get_bg_colors() {
  uint32_t v_result = g_bgcolors[g_bgcolor_index++];
  
  if ( g_bgcolor_index >= (sizeof(g_bgcolors)/sizeof(g_bgcolors[0])) ) {
    g_bgcolor_index = 0;
  }
  
  v_result |= g_bgcolors[g_bgcolor_index] << 16;
  
  return v_result;
}


void init_RENDER() {
}


static void render_clock() {
  struct tm v_tm;
  char v_str[32];
  
  time_t v_time = ( g_rtc_initialized ? rtc_get_cnt(g_time) : g_time ) + g_time_zone;
  gmtime_r( &v_time, &v_tm );
  // каждую минуту меняем пару цветов фона
  if ( v_tm.tm_min != g_last_tm_min ) {
    g_last_tm_min = v_tm.tm_min;
    g_bgcolors_current = get_bg_colors();
  }
  // отображение времени
  snprintf( v_str, sizeof(v_str), "%d:%02d", v_tm.tm_hour, v_tm.tm_min );
  diplay_write_string_with_background(
      0
    , 0
    , DISPLAY_WIDTH
    , font_110_110_font.m_row_height
    , v_str
    , &font_110_110_font
    , DISPLAY_COLOR_WHITE
    , g_bgcolors_current >> 16
    , g_bgcolors_current
    , v_tm.tm_sec
    );
  // отображение даты
  int v_y = font_110_110_font.m_row_height;
  snprintf( v_str, sizeof(v_str), "%d %s %d", v_tm.tm_mday, g_month_names[g_lang_index][v_tm.tm_mon % 12], v_tm.tm_year + 1900 );
  diplay_write_string_with_background(
      0
    , v_y
    , DISPLAY_WIDTH
    , font_28_32_font.m_row_height
    , v_str
    , &font_28_32_font
    , DISPLAY_COLOR_YELLOW
    , DISPLAY_COLOR_DARKGRAY
    , DISPLAY_COLOR_DARKGRAY
    , 0
    );
  v_y += font_28_32_font.m_row_height;
  // значок доступности времени от GNSS
  if ( g_gnss_ok ) {
    display_draw_zic_image( 0, v_y, Ignss_ok_tga_width, Ignss_ok_tga_height, Ignss_ok_tga_zic, sizeof(Ignss_ok_tga_zic) );
  } else {
    display_draw_zic_image( 0, v_y, Ignss_no_tga_width, Ignss_no_tga_height, Ignss_no_tga_zic, sizeof(Ignss_no_tga_zic) );
  }
  // значок будильника
  display_draw_zic_image(
      DISPLAY_WIDTH - Ialarm_clock_tga_width, v_y
    , Ialarm_clock_tga_width, Ialarm_clock_tga_height
    , Ialarm_clock_tga_zic
    , sizeof(Ialarm_clock_tga_zic)
    );
  v_y += Ialarm_clock_tga_height;
  // значок настроек
  display_draw_zic_image(
      DISPLAY_WIDTH - Isettings_tga_width, v_y
    , Isettings_tga_width, Isettings_tga_height
    , Isettings_tga_zic
    , sizeof(Isettings_tga_zic)
    );
  // отладка - вывод состояние BME280
  display_write_string( 48, 200, BMP280_detected() ? "вме280 работает" : "вме280 ошибка", &font_28_32_font, DISPLAY_COLOR_CYAN, DISPLAY_COLOR_DARKBLUE );
}


static void render_alarm() {
}


static void render_settings() {
}


void time_slice_RENDER() {
  if ( g_rended_mode_last != g_render_mode ) {
    // clear screen with black
    display_fill_rectangle_dma( 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_COLOR_BLACK );
    g_rended_mode_last = g_render_mode;
  }
  
  switch ( g_render_mode ) {
    case RENDER_MODE_ALARM:
      render_alarm();
      break;
      
    case RENDER_MODE_SETTINGS:
      render_settings();
      break;
      
    default:
      render_clock();
      break;
  }
}
