#ifndef _RTC_H_
#define _RTC_H_

#include <stdbool.h>
#include <stdint.h>

#define UTS_2024_01_01_00_00_00     1704067200u

void init_RTC();
void rtc_set_cnt( uint32_t a_def_value );
uint32_t rtc_get_cnt();

extern bool g_rtc_initialized;

#endif // _RTC_H_
