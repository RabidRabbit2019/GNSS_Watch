# На основе GD32VF103 baremetal software "library"
Часы с получением времени от модуля GNSS.

## Сборка
make

## Использованные выводы
PB8 (I2C0_SCL), PB9 (I2C0_SDA) - подключение датчика BME280  
PB11 (PENIRQ), PB12 (CS), PB13 (SPI1_SCK), PB14 (SPI1_MISO), PB15 (SPI1_MOSI) - подключение контроллера тачскрина XPT2046  
PA0 (RST), PA3 (DC), PA4 (CS), PA5 (SPI0_SCK), PA7 (SPI0_MOSI), PA8 (BL) - подключение экрана 320x240 с контроллером ILI9341  
PB7 (USART0_RX), PB6 (USART0_TX) - подключение модуля GNSS  
