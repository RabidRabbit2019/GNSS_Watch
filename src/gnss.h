#ifndef _GNSS_WATCH_GNSS_H_
#define _GNSS_WATCH_GNSS_H_


#include <time.h>
#include <stdbool.h>

void init_GNSS();
void time_slice_GNSS();

extern time_t g_time;
extern time_t g_time_zone;
extern bool g_gnss_ok;

#endif // _GNSS_WATCH_GNSS_H_
