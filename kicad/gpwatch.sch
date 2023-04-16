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
P 10150 2200
F 0 "U?" H 10150 -1050 60  0000 C CNN
F 1 "ER-TFTM032-3" H 10100 3050 60  0000 C CNN
F 2 "" H 10150 2200 60  0001 C CNN
F 3 "" H 10150 2200 60  0001 C CNN
	1    10150 2200
	1    0    0    -1  
$EndComp
$Comp
L GNSS-module U?
U 1 1 643C5C28
P 5350 5050
F 0 "U?" H 5350 4750 60  0000 C CNN
F 1 "GNSS-module" H 5350 5350 60  0000 C CNN
F 2 "" H 5350 5050 60  0001 C CNN
F 3 "" H 5350 5050 60  0001 C CNN
	1    5350 5050
	1    0    0    -1  
$EndComp
$Comp
L GY-BME280 U?
U 1 1 643C5D95
P 2250 2300
F 0 "U?" H 2250 1950 60  0000 C CNN
F 1 "GY-BME280" H 2250 2650 60  0000 C CNN
F 2 "" H 2250 2300 60  0001 C CNN
F 3 "" H 2250 2300 60  0001 C CNN
	1    2250 2300
	1    0    0    -1  
$EndComp
Text Label 6400 3550 2    60   ~ 0
3V3
Text Label 4300 4850 0    60   ~ 0
3V3
Text Label 8850 1550 0    60   ~ 0
3V3
Wire Wire Line
	6400 3550 5900 3550
Wire Wire Line
	4300 4850 4800 4850
Wire Wire Line
	8850 1550 9500 1550
Wire Wire Line
	3300 2050 2750 2050
Text Label 3300 2050 2    60   ~ 0
3V3
Wire Wire Line
	4250 2950 4800 2950
Text Label 4250 2950 0    60   ~ 0
SCK
Wire Wire Line
	4250 3050 4800 3050
Wire Wire Line
	4250 3150 4800 3150
Text Label 4250 3050 0    60   ~ 0
MISO
Text Label 4250 3150 0    60   ~ 0
MOSI
Wire Wire Line
	8950 3650 9500 3650
Wire Wire Line
	8950 3750 9500 3750
Wire Wire Line
	8950 4050 9500 4050
Wire Wire Line
	8950 4150 9500 4150
Text Label 8950 3750 0    60   ~ 0
SCK
Text Label 8950 4050 0    60   ~ 0
MOSI
Text Label 8950 4150 0    60   ~ 0
MISO
Wire Wire Line
	4250 2850 4800 2850
Text Label 4250 2850 0    60   ~ 0
CS
Text Label 8950 3650 0    60   ~ 0
CS
Wire Wire Line
	4250 2650 4800 2650
Wire Wire Line
	4250 2750 4800 2750
Text Label 4250 2650 0    60   ~ 0
MTX
Text Label 4250 2750 0    60   ~ 0
MRX
Wire Wire Line
	4300 5050 4800 5050
Wire Wire Line
	4300 5150 4800 5150
Text Label 4300 5050 0    60   ~ 0
MRX
Text Label 4300 5150 0    60   ~ 0
MTX
Wire Wire Line
	6400 2450 5900 2450
Wire Wire Line
	6400 2550 5900 2550
Text Label 6400 2450 2    60   ~ 0
SDA
Text Label 6400 2550 2    60   ~ 0
SCL
Wire Wire Line
	3300 2250 2750 2250
Wire Wire Line
	3300 2350 2750 2350
Text Label 3300 2250 2    60   ~ 0
SCL
Text Label 3300 2350 2    60   ~ 0
SDA
$EndSCHEMATC
