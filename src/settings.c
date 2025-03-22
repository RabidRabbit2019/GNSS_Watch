#include "settings.h"
#include "gd32vf103.h"

#include <string.h>


extern volatile uint32_t g_milliseconds;
void delay_ms( uint32_t a_ms );


static uint32_t crc32( touch_coeff_s * a_settings ) {
  a_settings->crc32 = 0;
  CRC_CTL = CRC_CTL_RST;
  while ( 0 != (CRC_CTL & CRC_CTL_RST) ) {}
  uint32_t * v_from = (uint32_t *)a_settings;
  for ( int i = 0; i < (int)(sizeof(touch_coeff_s) / sizeof(uint32_t)); ++i ) {
    // новое значение
    CRC_DATA = *v_from++;
    // гарантированные 4 такта шины AHB
    CRC_CTL;
    CRC_CTL;
    CRC_CTL;
    CRC_CTL;
  }
  // возвращаем результат
  return CRC_DATA;
}


static uint32_t settings_page_addr() {
  uint32_t v_flash_pages = FMC_SIZE - 1u;
  return FLASH_BASE + (v_flash_pages * FLASH_PAGE_SIZE);
}


static bool flash_page_erased() {
  uint32_t * v_ptr = (uint32_t *)settings_page_addr(); 
  for ( int i = 0; i < (int)WORDS_IN_PAGE; ++i ) {
    if ( 0xFFFFFFFFu != *v_ptr++ ) {
      return false;
    }
  }
  return true;
}


static bool unlock_FMC() {
  if ( 0 != (FMC_CTL & FMC_CTL_LK) ) {
    // флэш-контроллер заблокирован, пробуем разблокировать
    FMC_KEY = 0x45670123;
    FMC_KEY = 0xCDEF89AB;
    if ( 0 != (FMC_CTL & FMC_CTL_LK) ) {
      // всё ещё заблокирован - вот хрень
      return false;
    }
  }
  return true;
}


static bool erase_flash_page() {
  if ( !unlock_FMC() ) {
    return false;
  }
  // стираем страницу
  // сначала ждём освобождения флэш-контроллера
  uint32_t v_from = g_milliseconds;
  while ( 0 != (FMC_STAT & FMC_STAT_BUSY) ) {
    if ( ((uint32_t)(g_milliseconds - v_from)) > 500 ) {
      return false;
    }
  }
  // сбрасываем флаги ошибок и флаг завершения операции
  FMC_STAT |= ( FMC_STAT_PGERR | FMC_STAT_WPERR | FMC_STAT_ENDF );
  // выбираем стирание страницы
  FMC_CTL |= FMC_CTL_PER;
  // адрес страницы (последняя страница флэша)
  FMC_ADDR = settings_page_addr();
  // пуск!
  FMC_CTL |= FMC_CTL_START;
  // ждём завершения стирания
  v_from = g_milliseconds;
  while ( 0 != (FMC_STAT & FMC_STAT_BUSY) ) {
    if ( ((uint32_t)(g_milliseconds - v_from)) > 500 ) {
      return false;
    }
  }
  //
  return flash_page_erased();
}


static bool zap_settings_page() {
  // сначала проверим, может быть страница уже стёрта
  if ( !flash_page_erased() ) {
    return erase_flash_page();
  } else {
    return true;
  }
}


// адрес записи должен быть выровнен по границе 4 байт
// a_count - количество 32-битных слов для записи
static void write_flash( uint32_t * a_dst, const uint32_t * a_src, uint32_t a_count ) {
  if ( !unlock_FMC() ) {
    return;
  }
  // пишем по 64 бита
  // сначала ждём освобожения флэш-контроллера
  uint32_t v_from = g_milliseconds;
  while ( 0 != (FMC_STAT & FMC_STAT_BUSY) ) {
    if ( ((uint32_t)(g_milliseconds - v_from)) > 50 ) {
      return;
    }
  }
  //
  while ( 0 != a_count ) {
    // сбрасываем флаги ошибок и флаг завершения операции
    FMC_STAT |= ( FMC_STAT_PGERR | FMC_STAT_WPERR | FMC_STAT_ENDF );
    // выбираем запись
    FMC_CTL |= FMC_CTL_PG;
    // пишем 32-битное слово
    *a_dst++ = *a_src++;
    // ждём завершения записи
    v_from = g_milliseconds;
    while ( 0 != (FMC_STAT & FMC_STAT_BUSY) ) {
      if ( ((uint32_t)(g_milliseconds - v_from)) > 10 ) {
        return;
      }
    }
    //
    --a_count;
  }
}


// сохранить коэффициенты калибровки тачскрина
void store_coeff( touch_coeff_s * a_profile ) {
  // посчитаем CRC32
  a_profile->crc32 = crc32( a_profile );
  // запишем в последнюю страницу флэша
  if ( zap_settings_page() ) {
    write_flash( (uint32_t *)settings_page_addr(), (const uint32_t *)a_profile, sizeof(touch_coeff_s) );
  }
  // залочить флэш
  FMC_CTL |= FMC_CTL_LK;
}


// инициализация
void settings_init() {
  // включаем блок CRC
  RCU_AHBEN |= RCU_AHBEN_CRCEN;
}


// загрузить коэффициенты калибровки тачскрина
bool load_coeff( touch_coeff_s * a_dst ) {
  memcpy( a_dst, (void *)settings_page_addr(), sizeof(touch_coeff_s) );
  uint32_t v_crc = a_dst->crc32;
  return crc32( a_dst ) == v_crc;
}
