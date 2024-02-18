#include "bme280.h"
#include "gd32vf103.h"

#include <stdint.h>


#define  BMP280_CHIP_ID               0x58
#define  BME280_CHIP_ID               0x60


#define  BMP280_REGISTER_DIG_T1       0x88
#define  BMP280_REGISTER_DIG_T2       0x8A
#define  BMP280_REGISTER_DIG_T3       0x8C
#define  BMP280_REGISTER_DIG_P1       0x8E
#define  BMP280_REGISTER_DIG_P2       0x90
#define  BMP280_REGISTER_DIG_P3       0x92
#define  BMP280_REGISTER_DIG_P4       0x94
#define  BMP280_REGISTER_DIG_P5       0x96
#define  BMP280_REGISTER_DIG_P6       0x98
#define  BMP280_REGISTER_DIG_P7       0x9A
#define  BMP280_REGISTER_DIG_P8       0x9C
#define  BMP280_REGISTER_DIG_P9       0x9E
#define  BME280_REGISTER_DIG_H1       0xA1
#define  BME280_REGISTER_DIG_H2       0xE1
#define  BME280_REGISTER_DIG_H3       0xE3
#define  BME280_REGISTER_DIG_H45      0xE4
#define  BME280_REGISTER_DIG_H6       0xE7
#define  BMP280_REGISTER_CHIPID       0xD0
#define  BMP280_REGISTER_VERSION      0xD1
#define  BMP280_REGISTER_SOFTRESET    0xE0
#define  BMP280_REGISTER_CAL26        0xE1 /**< R calibration = 0xE1-0xF0 */
#define  BME280_REGISTER_CONTROLHUMID 0xF2
#define  BMP280_REGISTER_STATUS       0xF3
#define  BMP280_REGISTER_CONTROL      0xF4
#define  BMP280_REGISTER_CONFIG       0xF5
#define  BMP280_REGISTER_PRESSUREDATA 0xF7
#define  BMP280_REGISTER_TEMPDATA     0xFA


#define  BMP280_SAMPLING_TEMP_POS     5
#define  BMP280_SAMPLING_PRESS_POS    2
#define  BME280_SAMPLING_HUM_POS      0
#define  BMP280_MODE_POS              0
#define  BMP280_STANDBY_POS           5
#define  BMP280_FILTER_POS            2
#define  BMP280_SPI_ENABLE_POS        0


// No sampling.
#define  BMP280_SAMPLING_NONE         0x00
// 1 sample.
#define  BMP280_SAMPLING_X1           0x01
// 2x over-sampling.
#define  BMP280_SAMPLING_X2           0x02
// 4x over-sampling.
#define  BMP280_SAMPLING_X4           0x03
// 8x over-sampling.
#define  BMP280_SAMPLING_X8           0x04
// 16x over-sampling.
#define  BMP280_SAMPLING_X16          0x05


// Sleep mode.
#define  BMP280_MODE_SLEEP            0x00
// Forced mode.
#define  BMP280_MODE_FORCED           0x01
// Normal mode.
#define  BMP280_MODE_NORMAL           0x03
// Software reset.
#define  BMP280_SOFT_RESET_CODE       0xB6


// No filtering.
#define  BMP280_FILTER_OFF            0x00
// 2x filtering.
#define  BMP280_FILTER_X2             0x01
// 4x filtering.
#define  BMP280_FILTER_X4             0x02
// 8x filtering.
#define  BMP280_FILTER_X8             0x03
// 16x filtering.
#define  BMP280_FILTER_X16            0x04


// 0.5 ms standby.
#define  BMP280_STANDBY_MS_1 0x00
// 62.5 ms standby.
#define  BMP280_STANDBY_MS_63 0x01
// 125 ms standby.
#define  BMP280_STANDBY_MS_125 0x02
// 250 ms standby.
#define  BMP280_STANDBY_MS_250 0x03
// 500 ms standby.
#define  BMP280_STANDBY_MS_500 0x04
// 1000 ms standby.
#define  BMP280_STANDBY_MS_1000 0x05
// 2000 ms standby.
#define  BMP280_STANDBY_MS_2000 0x06
// 4000 ms standby.
#define  BMP280_STANDBY_MS_4000 0x07


#define BMP280_WRITE_ADDR             0xEC
#define BMP280_READ_ADDR              0xED

#define I2C_CLOCK_SPEED   100000


#pragma pack(push, 1)
typedef struct {
  uint16_t dig_T1; ///< temperature compensation value
  int16_t dig_T2;  ///< temperature compensation value
  int16_t dig_T3;  ///< temperature compensation value

  uint16_t dig_P1; ///< pressure compensation value
  int16_t dig_P2;  ///< pressure compensation value
  int16_t dig_P3;  ///< pressure compensation value
  int16_t dig_P4;  ///< pressure compensation value
  int16_t dig_P5;  ///< pressure compensation value
  int16_t dig_P6;  ///< pressure compensation value
  int16_t dig_P7;  ///< pressure compensation value
  int16_t dig_P8;  ///< pressure compensation value
  int16_t dig_P9;  ///< pressure compensation value

  int16_t dig_H2; ///< humidity compensation value
  int16_t dig_H4; ///< humidity compensation value
  int16_t dig_H5; ///< humidity compensation value
  uint8_t dig_H1; ///< humidity compensation value
  uint8_t dig_H3; ///< humidity compensation value
  int8_t dig_H6;  ///< humidity compensation value
} bmp280_calib_data;
#pragma pack(pop)

typedef struct {
  uint8_t dig_H1; ///< humidity compensation value
  int16_t dig_H2; ///< humidity compensation value
  uint8_t dig_H3; ///< humidity compensation value
  int16_t dig_H4; ///< humidity compensation value
  int16_t dig_H5; ///< humidity compensation value
  int8_t dig_H6;  ///< humidity compensation value
} bme280_calib_data;

static bmp280_calib_data g_bmp280_cd;
static bme280_calib_data g_bme280_cd;

static bool g_bme_detected = false;
static bool g_is_bme = false;

void delay_ms( uint32_t a_ms );


bool BMP280_is_BME() {
  return g_is_bme;
}


bool BMP280_detected() {
  return g_bme_detected;
}


// ожидание флага в регистре STAT0
static bool BMP280_wait_for_STAT0_FLAG(uint32_t a_flag) {
  uint32_t stat0;

  //
  uint32_t v_from = g_milliseconds;
  do {
    stat0 = I2C_STAT0(I2C0);
    I2C_STAT1(I2C0);
    if ( 0 != (stat0 & a_flag) ) {
      // flag set
      return true;
    }
  } while ( ((uint32_t)(g_milliseconds - v_from)) < 10u );
  // не дождались установки флага
  return false;
}


// отправить адрес устройства
static bool BMP280_set_addr( uint8_t a_addr ) {
  // make START
  I2C_CTL0(I2C0) |= I2C_CTL0_START;
  // wait for START condition
  if ( !BMP280_wait_for_STAT0_FLAG(I2C_STAT0_SBSEND) ) {
    return false;
  }
  // start transmit addr
  I2C_DATA(I2C0) = a_addr;  
  // wait for address transmission ends
  if ( !BMP280_wait_for_STAT0_FLAG(I2C_STAT0_ADDSEND) ) {
    return false;
  }
  return true;
}


static bool BMP280_wait_stop() {
  // wait for STOP condition
  uint32_t v_from = g_milliseconds;
  do {
    if ( 0 == (I2C_CTL0(I2C0) & I2C_CTL0_STOP) ) {
      // set ACK
      I2C_CTL0(I2C0) |= I2C_CTL0_ACKEN;
      return true;
    }
  } while ( ((uint32_t)(g_milliseconds - v_from)) < 10u );
  //
  return false;
}


static bool BMP280_write8( uint8_t a_reg, uint8_t a_value ) {
  if ( !BMP280_set_addr( BMP280_WRITE_ADDR ) ) {
    return false;
  }
  // send BME280 register number
  I2C_DATA(I2C0) = a_reg;
  // wait for TxE
  if ( !BMP280_wait_for_STAT0_FLAG(I2C_STAT0_TBE) ) {
    return false;
  }
  // send BME280 register value
  I2C_DATA(I2C0) = a_value;
  // wait for byte sent to shift reg
  if ( !BMP280_wait_for_STAT0_FLAG(I2C_STAT0_BTC) ) {
    return false;
  }
  // make STOP
  I2C_CTL0(I2C0) |= I2C_CTL0_STOP;
  //  
  return BMP280_wait_stop();
}


static bool BMP280_read8( uint8_t * a_dst, uint8_t a_reg ) {
  if ( !BMP280_set_addr( BMP280_WRITE_ADDR ) ) {
    return false;
  }
  // send BME280 register number
  I2C_DATA(I2C0) = a_reg;
  // wait for TxE
  if ( !BMP280_wait_for_STAT0_FLAG(I2C_STAT0_TBE) ) {
    return false;
  }
  // set read addr
  if ( !BMP280_set_addr( BMP280_READ_ADDR ) ) {
    return false;
  }
  // reset ACK, set STOP, for reading 1 byte
  I2C_CTL0(I2C0) = (I2C_CTL0(I2C0) & ~(I2C_CTL0_ACKEN)) | I2C_CTL0_STOP;
  // wait for reading byte
  if ( !BMP280_wait_for_STAT0_FLAG(I2C_STAT0_RBNE) ) {
    return false;
  }
  // read
  *a_dst = I2C_DATA(I2C0);
  // wait for STOP cleared
  return BMP280_wait_stop();
}


static bool BMP280_readN_intl( uint8_t * a_dst, uint8_t a_reg, int a_count ) {
  if ( !BMP280_set_addr( BMP280_WRITE_ADDR ) ) {
    return false;
  }
  // send BME280 register number
  I2C_DATA(I2C0) = a_reg;
  // wait for TxE
  if ( !BMP280_wait_for_STAT0_FLAG(I2C_STAT0_TBE) ) {
    return false;
  }
  // set read addr
  if ( !BMP280_set_addr( BMP280_READ_ADDR ) ) {
    return false;
  }
  // AN2824 I2C optimized
  for ( ; a_count > 3; --a_count ) {
    // wait for RXNE
    if ( !BMP280_wait_for_STAT0_FLAG(I2C_STAT0_RBNE) ) {
      return false;
    }
    *a_dst++ = I2C_DATA(I2C0);
  }
  // wait for RXNE
  if ( !BMP280_wait_for_STAT0_FLAG(I2C_STAT0_RBNE) ) {
    return false;
  }
  // wait for BTF
  if ( !BMP280_wait_for_STAT0_FLAG(I2C_STAT0_BTC) ) {
    return false;
  }
  // clear ACK
  I2C_CTL0(I2C0) &= ~I2C_CTL0_ACKEN;
  // read [count-2]
  *a_dst++ = I2C_DATA(I2C0);
  // set STOP
  I2C_CTL0(I2C0) |= I2C_CTL0_STOP;
  // read [count-1]
  *a_dst++ = I2C_DATA(I2C0);
  // wait for RXNE
  if ( !BMP280_wait_for_STAT0_FLAG(I2C_STAT0_RBNE) ) {
    return false;
  }
  // read [count]
  *a_dst++ = I2C_DATA(I2C0);
  // wait for STOP cleared
  return BMP280_wait_stop();
}


static bool BMP280_readN( uint8_t * a_dst, uint8_t a_reg, int a_count ) {
  if ( a_count > 2 ) {
    return BMP280_readN_intl( a_dst, a_reg, a_count );
  } else {
    while ( a_count > 0 ) {
      if ( !BMP280_read8( a_dst++, a_reg++ ) ) {
        return false;
      }
      --a_count;
    }
    return true;
  }
}


static bool BMP280_read64( int32_t * a_dst_t, int32_t * a_dst_p, int32_t * a_dst_h ) {
  uint8_t r64[8];
  if ( !BMP280_readN( r64, BMP280_REGISTER_PRESSUREDATA, g_is_bme ? 8 : 6 ) ) {
    return false;
  }
  *a_dst_p = (r64[0] << 12) | (r64[1] << 4) | (r64[2] >> 4);
  *a_dst_t = (r64[3] << 12) | (r64[4] << 4) | (r64[5] >> 4);
  *a_dst_h = (r64[6] << 8) | r64[7];
  return true;
}


static bool BMP280_read_calib_data(void) {
  uint8_t v_bme_h45[3];
  if ( !BMP280_readN( (uint8_t *)&g_bmp280_cd, BMP280_REGISTER_DIG_T1, (int)sizeof(g_bmp280_cd) ) ) {
    return false;
  }
  if ( g_is_bme ) {
    if ( !BMP280_read8( (uint8_t *)&g_bme280_cd.dig_H1, BME280_REGISTER_DIG_H1 )
      || !BMP280_readN( (uint8_t *)&g_bme280_cd.dig_H2, BME280_REGISTER_DIG_H2, 2 )
      || !BMP280_read8( (uint8_t *)&g_bme280_cd.dig_H3, BME280_REGISTER_DIG_H3 )
      || !BMP280_readN( v_bme_h45, BME280_REGISTER_DIG_H45, 3 )
      || !BMP280_read8( (uint8_t *)&g_bme280_cd.dig_H6, BME280_REGISTER_DIG_H6 )
        ) {
      return false;
    }
    g_bme280_cd.dig_H4 = (((int16_t)v_bme_h45[0]) << 4) | (v_bme_h45[1] & 0x0F);
    g_bme280_cd.dig_H5 = (v_bme_h45[1] >> 4) | ((((int16_t)v_bme_h45[2]) << 4));
  }
  return true;
}


bool init_BMP280() {
  // включаем тактирование I2C0
  RCU_APB1EN |= RCU_APB1EN_I2C0EN;
  // перенос функций выводов I2C0 (PB9 - SDA, PB8 - SCL)
  // и настройка I2C выводов в main()
  // I2C0 настройка
  I2C_CTL0(I2C0) = 0;
  uint32_t v_cck_mhz = SystemAPB1Clock / 1000000u;
  I2C_CTL1(I2C0) = v_cck_mhz;
  I2C_RT(I2C0) = v_cck_mhz + 1;
  I2C_CKCFG(I2C0) = (SystemAPB1Clock / 2) / I2C_CLOCK_SPEED;
  I2C_CTL0(I2C0) = I2C_CTL0_I2CEN;
  // if BUSY flag is set, try to reset and wait
  if ( 0 != (I2C_STAT1(I2C0) & I2C_STAT1_I2CBSY) ) {
    I2C_CTL0(I2C0) = I2C_CTL0_I2CEN | I2C_CTL0_SRESET;
    uint32_t v_from = g_milliseconds;
    while ( 0 != (I2C_STAT1(I2C0) & I2C_STAT1_I2CBSY) ) {
      if ( ((uint32_t)(g_milliseconds - v_from)) > 100 ) {
        // bus locked
        return false;
      }
    }
    I2C_CTL0(I2C0) = I2C_CTL0_I2CEN;
  }
  //
  uint8_t v_chipid;
  if ( !BMP280_read8( &v_chipid, BMP280_REGISTER_CHIPID ) ) {
    return false;
  }
  if ( BME280_CHIP_ID == v_chipid ) {
    g_is_bme = 1;
  } else {
    if ( BMP280_CHIP_ID != v_chipid ) {
      return false;
    }
  }
  //
  if ( !BMP280_write8(
        BMP280_REGISTER_SOFTRESET
      , BMP280_SOFT_RESET_CODE ) ) {
    return false;
  }
  delay_ms( 20 );
  if ( !BMP280_write8(BMP280_REGISTER_CONTROL, 0) ) {
    return false;
  }
  if ( g_is_bme ) {
    if ( !BMP280_write8(
          BME280_REGISTER_CONTROLHUMID
        , BMP280_SAMPLING_X16 << BME280_SAMPLING_HUM_POS
        ) ) {
      return false;
    }
  }
  if ( !BMP280_write8(
        BMP280_REGISTER_CONFIG
      , (BMP280_STANDBY_MS_63 << BMP280_STANDBY_POS)
        | (BMP280_FILTER_X16 << BMP280_FILTER_POS)
      ) ) {
    return false;
  }
  if ( !BMP280_write8(
        BMP280_REGISTER_CONTROL
      , (BMP280_SAMPLING_X2 << BMP280_SAMPLING_TEMP_POS)
        | (BMP280_SAMPLING_X16 << BMP280_SAMPLING_PRESS_POS)
        | (BMP280_MODE_NORMAL << BMP280_MODE_POS)
      ) ) {
    return false;
  }
  if ( !BMP280_read_calib_data() ) {
    return false;
  }
  delay_ms(100);
  g_bme_detected = true;
  return true;
}


static int32_t t_fine;

static int getTemperature(int32_t adc_T) {
  int32_t var1, var2;

  var1 = ((((adc_T >> 3) - ((int32_t)g_bmp280_cd.dig_T1 << 1))) *
          ((int32_t)g_bmp280_cd.dig_T2)) >>
         11;

  var2 = (((((adc_T >> 4) - ((int32_t)g_bmp280_cd.dig_T1)) *
            ((adc_T >> 4) - ((int32_t)g_bmp280_cd.dig_T1))) >>
           12) *
          ((int32_t)g_bmp280_cd.dig_T3)) >>
         14;

  t_fine = var1 + var2;

  return (t_fine * 5 + 128) >> 8;
}


static int getPressure(int32_t adc_P32) {
  int64_t var1, var2, p, adc_P;
  adc_P = adc_P32;

  var1 = ((int64_t)t_fine) - 128000;
  var2 = var1 * var1 * (int64_t)g_bmp280_cd.dig_P6;
  var2 = var2 + ((var1 * (int64_t)g_bmp280_cd.dig_P5) << 17);
  var2 = var2 + (((int64_t)g_bmp280_cd.dig_P4) << 35);
  var1 = ((var1 * var1 * (int64_t)g_bmp280_cd.dig_P3) >> 8) +
         ((var1 * (int64_t)g_bmp280_cd.dig_P2) << 12);
  var1 =
      (((((int64_t)1) << 47) + var1)) * ((int64_t)g_bmp280_cd.dig_P1) >> 33;

  if (var1 == 0) {
    return 0; // avoid exception caused by division by zero
  }
  p = 1048576 - adc_P;
  p = (((p << 31) - var2) * 3125) / var1;
  var1 = (((int64_t)g_bmp280_cd.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
  var2 = (((int64_t)g_bmp280_cd.dig_P8) * p) >> 19;

  p = ((p + var1 + var2) >> 8) + (((int64_t)g_bmp280_cd.dig_P7) << 4);
  return (int)(p >> 8);
}


static int getHumidity(int32_t adc_H) {
  if ((0 == g_is_bme) || (adc_H == 0x8000)) // value in case humidity measurement was disabled
    return 0;

  int32_t v_x1_u32r;

  v_x1_u32r = (t_fine - ((int32_t)76800));

  v_x1_u32r = (((((adc_H << 14) - (((int32_t)g_bme280_cd.dig_H4) << 20) -
                  (((int32_t)g_bme280_cd.dig_H5) * v_x1_u32r)) +
                 ((int32_t)16384)) >>
                15) *
               (((((((v_x1_u32r * ((int32_t)g_bme280_cd.dig_H6)) >> 10) *
                    (((v_x1_u32r * ((int32_t)g_bme280_cd.dig_H3)) >> 11) +
                     ((int32_t)32768))) >>
                   10) +
                  ((int32_t)2097152)) *
                     ((int32_t)g_bme280_cd.dig_H2) +
                 8192) >>
                14));

  v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) *
                             ((int32_t)g_bme280_cd.dig_H1)) >>
                            4));

  v_x1_u32r = (v_x1_u32r < 0) ? 0 : v_x1_u32r;
  v_x1_u32r = (v_x1_u32r > 419430400) ? 419430400 : v_x1_u32r;
  return v_x1_u32r >> 22;
}


bool BMP280_readMesure(int * a_dst_t, int * a_dst_p, int * a_dst_h) {
  int32_t adc_T, adc_P, adc_H;

  // Must be done first to get the t_fine variable set up
  if ( !BMP280_read64(&adc_T, &adc_P, &adc_H) ) {
    return false;
  }

  *a_dst_t = getTemperature(adc_T);
  *a_dst_p = getPressure(adc_P);
  *a_dst_h = getHumidity(adc_H);
  return true;
}
