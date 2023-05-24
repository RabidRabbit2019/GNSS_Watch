EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:gpwatch
LIBS:gpwatch-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L LonganNano U?
U 1 1 643C5A38
P 5350 2900
F 0 "U?" H 5350 2000 60  0000 C CNN
F 1 "LonganNano" H 5350 3850 60  0000 C CNN
F 2 "" H 5350 2900 60  0001 C CNN
F 3 "" H 5350 2900 60  0001 C CNN
	1    5350 2900
	1    0    0    -1  
$EndComp
$Comp
L ER-TFTM032-3 U?
U 1 1 643C5AC9
P 8200 2200
F 0 "U?" H 8200 -1050 60  0000 C CNN
F 1 "ER-TFTM032-3" H 8150 3050 60  0000 C CNN
F 2 "" H 8200 2200 60  0001 C CNN
F 3 "" H 8200 2200 60  0001 C CNN
	1    8200 2200
	1    0    0    -1  
$EndComp
$Comp
L GNSS-module U?
U 1 1 643C5C28
P 5350 4750
F 0 "U?" H 5350 4450 60  0000 C CNN
F 1 "GNSS-module" H 5350 5050 60  0000 C CNN
F 2 "" H 5350 4750 60  0001 C CNN
F 3 "" H 5350 4750 60  0001 C CNN
	1    5350 4750
	1    0    0    -1  
$EndComp
$Comp
L GY-BME280 U?
U 1 1 643C5D95
P 2650 2300
F 0 "U?" H 2650 1950 60  0000 C CNN
F 1 "GY-BME280" H 2650 2650 60  0000 C CNN
F 2 "" H 2650 2300 60  0001 C CNN
F 3 "" H 2650 2300 60  0001 C CNN
	1    2650 2300
	1    0    0    -1  
$EndComp
Text Label 6400 3550 2    60   ~ 0
3V3
Text Label 4300 4550 0    60   ~ 0
3V3
Text Label 6900 1550 0    60   ~ 0
3V3
Text Label 3700 2050 2    60   ~ 0
3V3
Text Label 4250 2950 0    60   ~ 0
SCK
Text Label 4250 3050 0    60   ~ 0
MISO
Text Label 4250 3150 0    60   ~ 0
MOSI
Text Label 7000 3750 0    60   ~ 0
SCK
Text Label 7000 4050 0    60   ~ 0
MOSI
Text Label 7000 4150 0    60   ~ 0
MISO
Text Label 4250 2850 0    60   ~ 0
CS
Text Label 7000 3650 0    60   ~ 0
CS
Text Label 6400 2550 2    60   ~ 0
MTX
Text Label 6400 2450 2    60   ~ 0
MRX
Text Label 4300 4750 0    60   ~ 0
MRX
Text Label 4300 4850 0    60   ~ 0
MTX
Text Label 6400 2250 2    60   ~ 0
SDA
Text Label 6400 2350 2    60   ~ 0
SCL
Text Label 3700 2250 2    60   ~ 0
SCL
Text Label 3700 2350 2    60   ~ 0
SDA
$Comp
L GND #PWR?
U 1 1 643CCD48
P 7450 5450
F 0 "#PWR?" H 7450 5200 50  0001 C CNN
F 1 "GND" H 7450 5300 50  0000 C CNN
F 2 "" H 7450 5450 50  0000 C CNN
F 3 "" H 7450 5450 50  0000 C CNN
	1    7450 5450
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 643CCE00
P 7450 1350
F 0 "#PWR?" H 7450 1100 50  0001 C CNN
F 1 "GND" H 7450 1200 50  0000 C CNN
F 2 "" H 7450 1350 50  0000 C CNN
F 3 "" H 7450 1350 50  0000 C CNN
	1    7450 1350
	-1   0    0    1   
$EndComp
$Comp
L GND #PWR?
U 1 1 643CCEC8
P 6000 1950
F 0 "#PWR?" H 6000 1700 50  0001 C CNN
F 1 "GND" H 6000 1800 50  0000 C CNN
F 2 "" H 6000 1950 50  0000 C CNN
F 3 "" H 6000 1950 50  0000 C CNN
	1    6000 1950
	-1   0    0    1   
$EndComp
$Comp
L GND #PWR?
U 1 1 643CCF63
P 3900 2250
F 0 "#PWR?" H 3900 2000 50  0001 C CNN
F 1 "GND" H 3900 2100 50  0000 C CNN
F 2 "" H 3900 2250 50  0000 C CNN
F 3 "" H 3900 2250 50  0000 C CNN
	1    3900 2250
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 643CCFFD
P 4100 4750
F 0 "#PWR?" H 4100 4500 50  0001 C CNN
F 1 "GND" H 4100 4600 50  0000 C CNN
F 2 "" H 4100 4750 50  0000 C CNN
F 3 "" H 4100 4750 50  0000 C CNN
	1    4100 4750
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X02 P?
U 1 1 644603B3
P 5000 1350
F 0 "P?" H 5000 1500 50  0000 C CNN
F 1 "CONN_01X02" V 5100 1350 50  0000 C CNN
F 2 "" H 5000 1350 50  0000 C CNN
F 3 "" H 5000 1350 50  0000 C CNN
	1    5000 1350
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 644606E5
P 4700 1500
F 0 "#PWR?" H 4700 1250 50  0001 C CNN
F 1 "GND" H 4700 1350 50  0000 C CNN
F 2 "" H 4700 1500 50  0000 C CNN
F 3 "" H 4700 1500 50  0000 C CNN
	1    4700 1500
	1    0    0    -1  
$EndComp
Text Label 4200 1300 0    60   ~ 0
BAT
Text Label 4250 2050 0    60   ~ 0
BAT
$Comp
L CONN_01X04 P?
U 1 1 644610D6
P 2450 3300
F 0 "P?" H 2450 3550 50  0000 C CNN
F 1 "CONN_01X04" V 2550 3300 50  0000 C CNN
F 2 "" H 2450 3300 50  0000 C CNN
F 3 "" H 2450 3300 50  0000 C CNN
	1    2450 3300
	-1   0    0    -1  
$EndComp
Wire Wire Line
	6400 3550 5900 3550
Wire Wire Line
	4300 4550 4800 4550
Wire Wire Line
	6900 1550 7550 1550
Wire Wire Line
	3700 2050 3150 2050
Wire Wire Line
	4250 2950 4800 2950
Wire Wire Line
	4250 3050 4800 3050
Wire Wire Line
	4250 3150 4800 3150
Wire Wire Line
	7000 3650 7550 3650
Wire Wire Line
	7000 3750 7550 3750
Wire Wire Line
	7000 4050 7550 4050
Wire Wire Line
	7000 4150 7550 4150
Wire Wire Line
	4250 2850 4800 2850
Wire Wire Line
	4300 4750 4800 4750
Wire Wire Line
	4300 4850 4800 4850
Wire Wire Line
	6400 2250 5900 2250
Wire Wire Line
	3700 2250 3150 2250
Wire Wire Line
	3700 2350 3150 2350
Wire Wire Line
	7450 5450 7450 5350
Wire Wire Line
	7450 5350 7550 5350
Wire Wire Line
	7550 1450 7450 1450
Wire Wire Line
	7450 1450 7450 1350
Wire Wire Line
	5900 2050 6000 2050
Wire Wire Line
	6000 1950 6000 2150
Wire Wire Line
	6000 2150 5900 2150
Connection ~ 6000 2050
Wire Wire Line
	3900 2250 3900 2150
Wire Wire Line
	3900 2150 3150 2150
Wire Wire Line
	4100 4750 4100 4650
Wire Wire Line
	4100 4650 4800 4650
Wire Wire Line
	4250 2050 4800 2050
Wire Wire Line
	4200 1300 4800 1300
Wire Wire Line
	4700 1500 4700 1400
Wire Wire Line
	4700 1400 4800 1400
Wire Wire Line
	3300 3150 2650 3150
Wire Wire Line
	3300 3250 2650 3250
Wire Wire Line
	3300 3350 2650 3350
$Comp
L GND #PWR?
U 1 1 644613F7
P 2750 3550
F 0 "#PWR?" H 2750 3300 50  0001 C CNN
F 1 "GND" H 2750 3400 50  0000 C CNN
F 2 "" H 2750 3550 50  0000 C CNN
F 3 "" H 2750 3550 50  0000 C CNN
	1    2750 3550
	1    0    0    -1  
$EndComp
Wire Wire Line
	2750 3550 2750 3450
Wire Wire Line
	2750 3450 2650 3450
Text Label 3300 3150 2    60   ~ 0
BT1
Text Label 3300 3250 2    60   ~ 0
BT2
Text Label 3300 3350 2    60   ~ 0
BT3
Wire Wire Line
	5900 3350 6900 3350
Wire Wire Line
	5900 3250 6700 3250
Wire Wire Line
	5900 3150 6500 3150
$Comp
L R R?
U 1 1 6446165E
P 6500 2900
F 0 "R?" V 6580 2900 50  0000 C CNN
F 1 "R" V 6500 2900 50  0000 C CNN
F 2 "" V 6430 2900 50  0000 C CNN
F 3 "" H 6500 2900 50  0000 C CNN
	1    6500 2900
	1    0    0    -1  
$EndComp
$Comp
L R R?
U 1 1 644616B6
P 6700 2900
F 0 "R?" V 6780 2900 50  0000 C CNN
F 1 "R" V 6700 2900 50  0000 C CNN
F 2 "" V 6630 2900 50  0000 C CNN
F 3 "" H 6700 2900 50  0000 C CNN
	1    6700 2900
	1    0    0    -1  
$EndComp
$Comp
L R R?
U 1 1 644616E2
P 6900 2900
F 0 "R?" V 6980 2900 50  0000 C CNN
F 1 "R" V 6900 2900 50  0000 C CNN
F 2 "" V 6830 2900 50  0000 C CNN
F 3 "" H 6900 2900 50  0000 C CNN
	1    6900 2900
	1    0    0    -1  
$EndComp
Wire Wire Line
	6500 3150 6500 3050
Wire Wire Line
	6700 3250 6700 3050
Wire Wire Line
	6900 3350 6900 3050
$Comp
L +3V3 #PWR?
U 1 1 644618C4
P 6900 2550
F 0 "#PWR?" H 6900 2400 50  0001 C CNN
F 1 "+3V3" H 6900 2690 50  0000 C CNN
F 2 "" H 6900 2550 50  0000 C CNN
F 3 "" H 6900 2550 50  0000 C CNN
	1    6900 2550
	1    0    0    -1  
$EndComp
Wire Wire Line
	6900 2750 6900 2550
Wire Wire Line
	6700 2750 6700 2650
Wire Wire Line
	6500 2650 6900 2650
Connection ~ 6900 2650
Wire Wire Line
	6500 2750 6500 2650
Connection ~ 6700 2650
Text Label 6400 3150 2    60   ~ 0
BT1
Text Label 6400 3250 2    60   ~ 0
BT2
Text Label 6400 3350 2    60   ~ 0
BT3
Wire Wire Line
	6400 2350 5900 2350
Wire Wire Line
	6400 2450 5900 2450
Wire Wire Line
	6400 2550 5900 2550
Wire Wire Line
	7000 3850 7550 3850
Text Label 7000 3850 0    60   ~ 0
LCD_DC
Wire Wire Line
	4250 2750 4800 2750
Text Label 4250 2750 0    60   ~ 0
LCD_DC
Wire Wire Line
	7000 3450 7550 3450
Text Label 7000 3450 0    60   ~ 0
LCD_RST
Wire Wire Line
	4250 2450 4800 2450
Text Label 4250 2450 0    60   ~ 0
LCD_RST
Wire Wire Line
	7000 4250 7550 4250
Text Label 7000 4250 0    60   ~ 0
BL_ON_OFF
Text Label 6400 3050 2    60   ~ 0
BL_ON_OFF
Wire Wire Line
	6400 3050 5900 3050
$EndSCHEMATC
