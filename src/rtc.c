#include "rtc.h"
#include "gd32vf103.h"

#include <string.h>


#define RTC_LWOFF_WAIT_TIMEOUT_MS   20u
#define RTC_RSYNF_WAIT_TIMEOUT_MS   1000u
#define RTC_LXTAL_TIMEOUT_MS        1000u
#define RTC_PRESCALER_VALUE         32767u
#define UTS_2024_01_01_00_00_00     1704067200u

#define RTC_HIGH_BITS_MASK          0x000F0000u
#define RTC_LOW_BITS_MASK           0x0000FFFFu
#define RTC_HIGH_BITS_OFFSET        16


static const uint16_t g_rtc_marker[4] = {
  0xDEAD
, 0xBEEF
, 0xFA11
, 0xCAFE
};


// признак успешной настройки RTC
bool g_rtc_initialized = false;

int g_rtc_err_code = 0;

// ожидание указанного флага в RTC_CTL не более RTC_FLAGS_WAIT_TIMEOUT_MS мс
static bool rtc_ctl_wait_flag( uint32_t a_flag, uint32_t a_timeout_ms )
{
  for ( uint32_t v_from = g_milliseconds; ((uint32_t)(g_milliseconds - v_from)) <= a_timeout_ms; ) {
    if ( 0 != (RTC_CTL & a_flag) ) {
      // дождались
      return true;
    }
  }
  return false;
}

//
static void rtc_disable( int a_line ) {
  RCU_BDCTL &= ~RCU_BDCTL_RTCEN;
  RCU_BDCTL &= ~RCU_BDCTL_LXTALEN;
  PMU_CTL &= ~PMU_CTL_BKPWEN;
  RCU_APB1EN &= ~(RCU_APB1EN_BKPIEN | RCU_APB1EN_PMUEN);
  g_rtc_initialized = false;
  g_rtc_err_code = a_line;
}


// настройка RTC
void init_RTC() {
  // включаем тактирование PMU, BKP
  RCU_APB1EN |= (RCU_APB1EN_BKPIEN | RCU_APB1EN_PMUEN);
  // читаем данные из регистров BKP
  uint16_t v_bkp[4];
  v_bkp[0] = BKP_DATA0;
  v_bkp[1] = BKP_DATA2;
  v_bkp[2] = BKP_DATA4;
  v_bkp[3] = BKP_DATA6;
  // проверяем маркер
  if ( 0 != memcmp( v_bkp, g_rtc_marker, sizeof(g_rtc_marker) ) ) {
    // сигнатура не совпадает, нужна инициализация RTC
    // включаем запись в регистры BKP (и RTC)
    PMU_CTL |= PMU_CTL_BKPWEN;
    // сброс BKP
    RCU_BDCTL |= RCU_BDCTL_BKPRST;
    RCU_BDCTL &= ~RCU_BDCTL_BKPRST;
    // включаем внешний генератор 32768Гц
    RCU_BDCTL |= RCU_BDCTL_LXTALEN;
    // ждём завершения его запуска (не более RTC_FLAGS_WAIT_TIMEOUT_MS мс)
    for ( uint32_t v_from = g_milliseconds;; ) {
      if ( 0 != (RCU_BDCTL & RCU_BDCTL_LXTALSTB) ) {
        // генератор запущен
        break;
      }
      if ( ((uint32_t)(g_milliseconds - v_from)) > RTC_LXTAL_TIMEOUT_MS ) {
        // явно не удалось
        rtc_disable(__LINE__);
        return;
      }
    }
    // установим источником тактового сигнала для RTC частоту с внешнего генератора на 32768Гц
    RCU_BDCTL = (RCU_BDCTL & ~RCU_BDCTL_RTCSRC) | RCU_RTCSRC_LXTAL;
    // включаем тактирование RTC
    RCU_BDCTL |= RCU_BDCTL_RTCEN;
    // ждём синхронизации регистров RTC, не более RTC_FLAGS_WAIT_TIMEOUT_MS мс
    RTC_CTL &= ~RTC_CTL_RSYNF;
    //
    if ( !rtc_ctl_wait_flag( RTC_CTL_RSYNF, RTC_RSYNF_WAIT_TIMEOUT_MS ) ) {
      // 
      rtc_disable(__LINE__);
      return;
    }
    // прописываем в RTC_PSCL делитель для получения частоты изменения RTC_CNT с частотой 1Гц
    // и количество секунд в RTC_CNT = timegm(2024-01-01 00:00:00)
    RTC_CTL |= RTC_CTL_CMF;
    RTC_PSCH = ((RTC_PRESCALER_VALUE & RTC_HIGH_BITS_MASK) >> RTC_HIGH_BITS_OFFSET);
    RTC_PSCL = RTC_PRESCALER_VALUE & RTC_LOW_BITS_MASK;
    RTC_CNTH = (UTS_2024_01_01_00_00_00 >> RTC_HIGH_BITS_OFFSET) & RTC_LOW_BITS_MASK;
    RTC_CNTL = UTS_2024_01_01_00_00_00 & RTC_LOW_BITS_MASK;
    RTC_CTL &= ~RTC_CTL_CMF;
    // ждём обновления настроек RTC
    if ( !rtc_ctl_wait_flag( RTC_CTL_LWOFF, RTC_LWOFF_WAIT_TIMEOUT_MS ) ) {
      // 
      rtc_disable(__LINE__);
      return;
    }
    // записываем маркер
    BKP_DATA0 = g_rtc_marker[0];
    BKP_DATA2 = g_rtc_marker[1];
    BKP_DATA4 = g_rtc_marker[2];
    BKP_DATA6 = g_rtc_marker[3];
  } else {
    // включаем запись в регистры BKP (и RTC)
    PMU_CTL |= PMU_CTL_BKPWEN;
    // ждём синхронизации регистров RTC, не более RTC_FLAGS_WAIT_TIMEOUT_MS мс
    RTC_CTL &= ~RTC_CTL_RSYNF;
    //
    if ( !rtc_ctl_wait_flag( RTC_CTL_RSYNF, RTC_RSYNF_WAIT_TIMEOUT_MS ) ) {
      // 
      rtc_disable(__LINE__);
      return;
    }
  }
  //
  g_rtc_initialized = true;
}


// установить счётчик секунд в RTC
void rtc_set_cnt( uint32_t a_cnt ) {
  RTC_CTL |= RTC_CTL_CMF;
  RTC_CNTH = (a_cnt >> RTC_HIGH_BITS_OFFSET) & RTC_LOW_BITS_MASK;
  RTC_CNTL = a_cnt & RTC_LOW_BITS_MASK;
  RTC_CTL &= ~RTC_CTL_CMF;
  // ждём обновления настроек RTC
  if ( !rtc_ctl_wait_flag( RTC_CTL_LWOFF, RTC_LWOFF_WAIT_TIMEOUT_MS ) ) {
    // 
    rtc_disable(__LINE__);
  }
}


// прочитать счётчик секунд из RTC
uint32_t rtc_get_cnt( uint32_t a_def_value ) {
  //
  uint32_t v_low = RTC_CNTL & RTC_LOW_BITS_MASK;
  uint32_t v_high = RTC_CNTH & RTC_LOW_BITS_MASK;
  //
  if ( v_low != (RTC_CNTL & RTC_LOW_BITS_MASK) ) {
    v_low = RTC_CNTL & RTC_LOW_BITS_MASK;
    v_high = RTC_CNTH & RTC_LOW_BITS_MASK;
  }
  //
  return v_low | (v_high << RTC_HIGH_BITS_OFFSET);
}
