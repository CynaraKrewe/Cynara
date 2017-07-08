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
LIBS:Connector
LIBS:TM4C
LIBS:IC
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
L TM4C1230H6PM U?
U 1 1 59613241
P 4100 5100
F 0 "U?" H 2850 7000 50  0000 L CNN
F 1 "TM4C1230H6PM" H 4750 7000 50  0000 L CNN
F 2 "LQFP-64" H 4100 6600 50  0001 C CIN
F 3 "" H 4100 5100 50  0001 C CNN
	1    4100 5100
	1    0    0    -1  
$EndComp
$Comp
L TLV62568 U1
U 1 1 5961380C
P 4550 1100
F 0 "U1" H 4600 1250 60  0000 C CNN
F 1 "TLV62568" H 4750 1150 60  0000 C CNN
F 2 "" H 4750 1150 60  0001 C CNN
F 3 "" H 4750 1150 60  0001 C CNN
	1    4550 1100
	1    0    0    -1  
$EndComp
$Comp
L C C1
U 1 1 596139D9
P 3350 1300
F 0 "C1" H 3375 1400 50  0000 L CNN
F 1 "4u7 10V" H 3375 1200 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 3388 1150 50  0001 C CNN
F 3 "" H 3350 1300 50  0001 C CNN
	1    3350 1300
	1    0    0    -1  
$EndComp
$Comp
L C C2
U 1 1 59613A40
P 5700 1300
F 0 "C2" H 5725 1400 50  0000 L CNN
F 1 "10uF 10V" H 5725 1200 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 5738 1150 50  0001 C CNN
F 3 "" H 5700 1300 50  0001 C CNN
	1    5700 1300
	1    0    0    -1  
$EndComp
$Comp
L R R1
U 1 1 59613BAF
P 6250 1300
F 0 "R1" V 6330 1300 50  0000 C CNN
F 1 "50k" V 6250 1300 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 6180 1300 50  0001 C CNN
F 3 "" H 6250 1300 50  0001 C CNN
	1    6250 1300
	1    0    0    -1  
$EndComp
$Comp
L R R2
U 1 1 59613C5E
P 6250 1700
F 0 "R2" V 6330 1700 50  0000 C CNN
F 1 "11k" V 6250 1700 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 6180 1700 50  0001 C CNN
F 3 "" H 6250 1700 50  0001 C CNN
	1    6250 1700
	1    0    0    -1  
$EndComp
$Comp
L L L1
U 1 1 59613D09
P 5300 1150
F 0 "L1" V 5250 1150 50  0000 C CNN
F 1 "2u2" V 5375 1150 50  0000 C CNN
F 2 "Inductors:Inductor_Taiyo-Yuden_MD-3030" H 5300 1150 50  0001 C CNN
F 3 "" H 5300 1150 50  0001 C CNN
	1    5300 1150
	0    1    1    0   
$EndComp
Text Notes 6500 1300 0    60   ~ 0
V_out = V_FB x ( 1 + R_FBT / R_FBB)
Text Notes 6600 1650 0    60   ~ 0
V_FB = 0.6V\nR_FBT = 11k\nR_FBB = 50k
$Comp
L GND #PWR?
U 1 1 596140AD
P 5700 1550
F 0 "#PWR?" H 5700 1300 50  0001 C CNN
F 1 "GND" H 5700 1400 50  0000 C CNN
F 2 "" H 5700 1550 50  0001 C CNN
F 3 "" H 5700 1550 50  0001 C CNN
	1    5700 1550
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 596140CF
P 5100 1550
F 0 "#PWR?" H 5100 1300 50  0001 C CNN
F 1 "GND" H 5100 1400 50  0000 C CNN
F 2 "" H 5100 1550 50  0001 C CNN
F 3 "" H 5100 1550 50  0001 C CNN
	1    5100 1550
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 59614168
P 3350 1550
F 0 "#PWR?" H 3350 1300 50  0001 C CNN
F 1 "GND" H 3350 1400 50  0000 C CNN
F 2 "" H 3350 1550 50  0001 C CNN
F 3 "" H 3350 1550 50  0001 C CNN
	1    3350 1550
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 59614201
P 6250 1900
F 0 "#PWR?" H 6250 1650 50  0001 C CNN
F 1 "GND" H 6250 1750 50  0000 C CNN
F 2 "" H 6250 1900 50  0001 C CNN
F 3 "" H 6250 1900 50  0001 C CNN
	1    6250 1900
	1    0    0    -1  
$EndComp
$Comp
L R R4
U 1 1 596152A0
P 4100 1450
F 0 "R4" V 4180 1450 50  0000 C CNN
F 1 "499k" V 4100 1450 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 4030 1450 50  0001 C CNN
F 3 "" H 4100 1450 50  0001 C CNN
	1    4100 1450
	0    1    1    0   
$EndComp
$Comp
L R R3
U 1 1 5961535C
P 4100 1300
F 0 "R3" V 4180 1300 50  0000 C CNN
F 1 "11k" V 4100 1300 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 4030 1300 50  0001 C CNN
F 3 "" H 4100 1300 50  0001 C CNN
	1    4100 1300
	0    1    1    0   
$EndComp
Wire Wire Line
	4450 1300 4250 1300
Wire Wire Line
	4250 1450 4450 1450
Wire Wire Line
	3350 1150 4450 1150
Wire Wire Line
	3350 1150 3350 1050
Wire Wire Line
	3950 1300 3850 1300
Wire Wire Line
	3850 1150 3850 1450
Connection ~ 3850 1150
Wire Wire Line
	3850 1450 3950 1450
Connection ~ 3850 1300
Wire Wire Line
	5050 1450 5100 1450
Wire Wire Line
	5100 1450 5100 1550
Wire Wire Line
	5050 1300 5300 1300
Wire Wire Line
	5300 1300 5300 1500
Wire Wire Line
	5300 1500 6250 1500
Wire Wire Line
	6250 1450 6250 1550
Connection ~ 6250 1500
Wire Wire Line
	5700 1550 5700 1450
Wire Wire Line
	5150 1150 5050 1150
Wire Wire Line
	5450 1150 6250 1150
Wire Wire Line
	3350 1450 3350 1550
Wire Wire Line
	6250 1850 6250 1900
Connection ~ 5700 1150
Wire Wire Line
	6250 1150 6250 1050
Wire Wire Line
	4350 1450 4350 1850
Wire Wire Line
	4350 1850 3900 1850
Connection ~ 4350 1450
Text GLabel 3900 1850 0    60   Input ~ 0
power_good
$Comp
L +3.3V #PWR?
U 1 1 59616F05
P 6250 1050
F 0 "#PWR?" H 6250 900 50  0001 C CNN
F 1 "+3.3V" H 6250 1190 50  0000 C CNN
F 2 "" H 6250 1050 50  0001 C CNN
F 3 "" H 6250 1050 50  0001 C CNN
	1    6250 1050
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR?
U 1 1 59617174
P 3350 1050
F 0 "#PWR?" H 3350 900 50  0001 C CNN
F 1 "+5V" H 3350 1190 50  0000 C CNN
F 2 "" H 3350 1050 50  0001 C CNN
F 3 "" H 3350 1050 50  0001 C CNN
	1    3350 1050
	1    0    0    -1  
$EndComp
Text Notes 3400 1100 0    60   ~ 0
max: 5.5V
Text Notes 5400 1100 0    60   ~ 0
I = 1.0 A (max)
$EndSCHEMATC
