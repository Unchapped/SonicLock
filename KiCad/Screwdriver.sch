EESchema Schematic File Version 2  date 8/8/2011 7:38:42 PM
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
LIBS:special
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
EELAYER 43  0
EELAYER END
$Descr A4 11700 8267
encoding utf-8
Sheet 1 1
Title ""
Date "9 aug 2011"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Connection ~ 4350 5200
Wire Wire Line
	4350 4650 4350 5200
Wire Wire Line
	3550 4150 3600 4150
Wire Wire Line
	3600 4150 3600 5200
Connection ~ 8000 5200
Wire Wire Line
	3600 5200 8000 5200
Connection ~ 4000 3950
Wire Wire Line
	5300 3850 4750 3850
Wire Wire Line
	7550 3750 7300 3750
Wire Wire Line
	7300 4250 8000 4250
Wire Wire Line
	8000 4250 8000 5300
Wire Wire Line
	8450 3450 8450 3750
Wire Wire Line
	8450 3750 8150 3750
Wire Wire Line
	3550 3950 5300 3950
Wire Wire Line
	4250 3850 4000 3850
Wire Wire Line
	4000 3850 4000 4250
Wire Wire Line
	4350 4250 5300 4250
Wire Wire Line
	4000 4650 4000 5200
Connection ~ 4000 5200
$Comp
L LED D1
U 1 1 4E4098E6
P 4350 4450
F 0 "D1" H 4350 4550 50  0000 C CNN
F 1 "LED" H 4350 4350 50  0000 C CNN
	1    4350 4450
	0    -1   1    0   
$EndComp
$Comp
L SPEAKER SP1
U 1 1 4E409832
P 3250 4050
F 0 "SP1" H 3150 4300 70  0000 C CNN
F 1 "SPEAKER" H 3150 3800 70  0000 C CNN
	1    3250 4050
	-1   0    0    -1  
$EndComp
$Comp
L C C1
U 1 1 4E4097D0
P 4000 4450
F 0 "C1" H 4050 4550 50  0000 L CNN
F 1 "C" H 4050 4350 50  0000 L CNN
	1    4000 4450
	1    0    0    -1  
$EndComp
$Comp
L R R1
U 1 1 4E40974D
P 4500 3850
F 0 "R1" V 4580 3850 50  0000 C CNN
F 1 "10K" V 4500 3850 50  0000 C CNN
	1    4500 3850
	0    -1   -1   0   
$EndComp
$Comp
L SW_PUSH SW1
U 1 1 4E409695
P 7850 3750
F 0 "SW1" H 8000 3860 50  0000 C CNN
F 1 "SW_PUSH" H 7850 3670 50  0000 C CNN
	1    7850 3750
	1    0    0    -1  
$EndComp
$Comp
L +3,3V #PWR01
U 1 1 4E40964D
P 8450 3450
F 0 "#PWR01" H 8450 3410 30  0001 C CNN
F 1 "+3,3V" H 8450 3560 30  0000 C CNN
	1    8450 3450
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR02
U 1 1 4E409632
P 8000 5300
F 0 "#PWR02" H 8000 5300 30  0001 C CNN
F 1 "GND" H 8000 5230 30  0001 C CNN
	1    8000 5300
	1    0    0    -1  
$EndComp
$Comp
L ATTINY13-P IC1
U 1 1 4E409247
P 5500 3650
F 0 "IC1" H 5600 3700 60  0000 C CNN
F 1 "ATTINY13-P" H 6800 2900 60  0000 C CNN
F 2 "DIP8" H 5600 2900 60  0001 C CNN
	1    5500 3650
	1    0    0    -1  
$EndComp
$EndSCHEMATC
