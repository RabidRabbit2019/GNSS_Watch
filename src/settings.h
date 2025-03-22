#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include <stdint.h>
#include <stdbool.h>

// структура для хранения в "EEPROM" коэффициентов калибровки тачскрина
typedef struct {
    int Ax;
    int Bx;
    int Dx;
    int Ay;
    int By;
    int Dy;
    uint32_t crc32;
} touch_coeff_s;


#define FLASH_PAGE_SIZE  1024
//
#define WORDS_IN_PAGE (FLASH_PAGE_SIZE/sizeof(uint32_t))

//
void settings_init();
// сохранить текущий профиль с указанным идентификатором
void store_coeff( touch_coeff_s * );
bool load_coeff( touch_coeff_s * );


#endif // __SETTINGS_H__
