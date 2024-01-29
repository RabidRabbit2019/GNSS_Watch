#ifndef _RTC_H_
#define _RTC_H_

#include <stdbool.h>
#include <stdint.h>


void init_RTC();
void rtc_set_cnt( uint32_t a_def_value );
uint32_t rtc_get_cnt();

extern bool g_rtc_initialized;
extern int g_rtc_err_code;

#endif // _RTC_H_
