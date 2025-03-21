#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include <font_bmp.h>

#include <stdint.h>
#include <stdbool.h>


#define DISPLAY_WIDTH             320
#define DISPLAY_HEIGHT            240
#define DISPLAY_MAX_LINE_PIXELS   DISPLAY_WIDTH
// максимальная длина выводимой на экран строки в символах
#define MAX_ONE_STR_SYMBOLS       24

#define RGB565(r,g,b) (((r&0xF8)<<8)|((g&0xFC)<<3)|(b>>3))
#define RGB565FINAL(r,g,b) ((r&0xF8)|((g&0xE0)>>5)|((g&0x1C)<<11)|((b&0xF8)<<5))

// Color definitions                       R    G    B
#define DISPLAY_COLOR_BLACK       RGB565(  0,   0,   0)
#define DISPLAY_COLOR_RED         RGB565(255,   0,   0)
#define DISPLAY_COLOR_GREEN       RGB565(  0, 255,   0)
#define DISPLAY_COLOR_BLUE        RGB565(  0,   0, 255)
#define DISPLAY_COLOR_CYAN        RGB565(  0, 255, 255)
#define DISPLAY_COLOR_MAGENTA     RGB565(255,   0, 255)
#define DISPLAY_COLOR_YELLOW      RGB565(255, 255,   0)
#define DISPLAY_COLOR_WHITE       RGB565(255, 255, 255)
#define DISPLAY_COLOR_GRAY        RGB565(127, 127, 127)
#define DISPLAY_COLOR_LIGHTGREY   RGB565(191, 191, 191)
#define DISPLAY_COLOR_PINK        RGB565(255, 191, 191)
#define DISPLAY_COLOR_DARKRED     RGB565(127,   0,   0)
#define DISPLAY_COLOR_DARKGREEN   RGB565(  0,  80,   0)
#define DISPLAY_COLOR_DARKBLUE    RGB565(  0,   0, 127)
#define DISPLAY_COLOR_DARKCYAN    RGB565(  0,  60,  80)
#define DISPLAY_COLOR_DARKMAGENTA RGB565(127,   0, 127)
#define DISPLAY_COLOR_DARKYELLOW  RGB565(127, 127,   0)
#define DISPLAY_COLOR_DARKGRAY    RGB565( 63,  63,  63)



void display_init_dma();
void display_fill_rectangle_dma( uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void display_draw_zic_image( int x, int y, int w, int h, const uint8_t * a_data, int a_data_len );
void display_draw_jpeg_image( int x, int y, const uint8_t * a_data, int a_data_len );
void display_write_string(uint16_t x, uint16_t y, const char* str, const packed_font_desc_s * fnt, uint16_t color, uint16_t bgcolor);
void display_write_char(uint16_t x, uint16_t y, display_char_s * a_data);
void diplay_write_string_with_background(
              int a_x
            , int a_y
            , int a_width
            , int a_height
            , const char * a_str
            , const packed_font_desc_s * a_fnt
            , uint16_t a_color
            , uint16_t a_bgcolor_1
            , uint16_t a_bgcolor_2
            , int a_seconds
            );
void display_select();
void display_deselect();

#endif // _DISPLAY_H_

