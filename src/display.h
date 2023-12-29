#ifndef _DISPLAY_H_
#define _DISPLAY_H_


#include <stdint.h>


#define DISPLAY_WIDTH             320
#define DISPLAY_HEIGHT            240
#define DISPLAY_MAX_LINE_PIXELS   DISPLAY_WIDTH


// Color definitions
#define DISPLAY_COLOR_BLACK       0x0000      /*   0,   0,   0 */
#define DISPLAY_COLOR_NAVY        0x000F      /*   0,   0, 128 */
#define DISPLAY_COLOR_DARKGREEN   0x03E0      /*   0, 128,   0 */
#define DISPLAY_COLOR_DARKCYAN    0x03EF      /*   0, 128, 128 */
#define DISPLAY_COLOR_MAROON      0x7800      /* 128,   0,   0 */
#define DISPLAY_COLOR_PURPLE      0x780F      /* 128,   0, 128 */
#define DISPLAY_COLOR_OLIVE       0x7BE0      /* 128, 128,   0 */
#define DISPLAY_COLOR_LIGHTGREY   0xC618      /* 192, 192, 192 */
#define DISPLAY_COLOR_DARKGREY    0x39E7      /* 128, 128, 128 */
#define DISPLAY_COLOR_BLUE        0x001F      /*   0,   0, 255 */
#define DISPLAY_COLOR_DARKBLUE    0x787F      /*   0,   0, 255 */
#define DISPLAY_COLOR_CYBLUE      0x03FF      /*   0,   0, 255 */
#define DISPLAY_COLOR_GREEN       0x07E0      /*   0, 255,   0 */
#define DISPLAY_COLOR_CYAN        0x07FF      /*   0, 255, 255 */
#define DISPLAY_COLOR_RED         0xF800      /* 255,   0,   0 */
#define DISPLAY_COLOR_MAGENTA     0xF81F      /* 255,   0, 255 */
#define DISPLAY_COLOR_YELLOW      0xFFE0      /* 255, 255,   0 */
#define DISPLAY_COLOR_WHITE       0xFFFF      /* 255, 255, 255 */
#define DISPLAY_COLOR_ORANGE      0xFD20      /* 255, 165,   0 */
#define DISPLAY_COLOR_GREENYELLOW 0xAFE5      /* 173, 255,  47 */
#define DISPLAY_COLOR_PINK        0xF81F



void display_init();
void display_init_dma();
void display_fill_rectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void display_fill_rectangle_dma( uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);


#endif // _DISPLAY_H_

