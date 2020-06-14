EESchema Schematic File Version 4
LIBS:bluetoothMidiController-cache
EELAYER 26 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 2 2
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
L bluetoothMidiController-rescue:FEATHERWING-adafruit MS?
U 1 1 5CBD227F
P 6400 2650
AR Path="/5CBD227F" Ref="MS?"  Part="1" 
AR Path="/5CBC7538/5CBD227F" Ref="MS1"  Part="1" 
F 0 "MS1" H 6400 2650 45  0001 C CNN
F 1 "FEATHERWING" H 6400 2650 45  0001 C CNN
F 2 "adafruit:FEATHERWING" H 6400 2650 50  0001 C CNN
F 3 "" H 6400 2650 50  0001 C CNN
	1    6400 2650
	-1   0    0    1   
$EndComp
NoConn ~ 5800 4150
NoConn ~ 5600 4150
NoConn ~ 5500 4150
NoConn ~ 4700 2450
NoConn ~ 4800 2450
NoConn ~ 4900 2450
NoConn ~ 5000 2450
NoConn ~ 5400 2450
NoConn ~ 5500 2450
NoConn ~ 6000 2450
NoConn ~ 6200 2450
Text Notes 4900 3300 0    50   ~ 0
Feather 32u4 Bluefruit LE
NoConn ~ 5600 2450
NoConn ~ 5700 2450
NoConn ~ 5800 2450
NoConn ~ 4900 4150
NoConn ~ 5000 4150
NoConn ~ 5100 4150
$Comp
L Connector:Conn_01x08_Female J?
U 1 1 5CBD2297
P 7350 3100
AR Path="/5CBD2297" Ref="J?"  Part="1" 
AR Path="/5CBC7538/5CBD2297" Ref="J2"  Part="1" 
F 0 "J2" H 7377 3076 50  0000 L CNN
F 1 "Conn_01x08_Female" H 7377 2985 50  0000 L CNN
F 2 "" H 7350 3100 50  0001 C CNN
F 3 "~" H 7350 3100 50  0001 C CNN
	1    7350 3100
	1    0    0    -1  
$EndComp
Text GLabel 7000 2800 0    50   Input ~ 0
+3V
Text GLabel 7000 2900 0    50   Input ~ 0
GND
Text GLabel 7000 3200 0    50   Input ~ 0
GPIO10
Text GLabel 7000 3300 0    50   Input ~ 0
GPIO11
Text GLabel 7000 3400 0    50   Input ~ 0
GPIO12
Text GLabel 7000 3000 0    50   Input ~ 0
SDA
Text GLabel 7000 3100 0    50   Input ~ 0
SCL
Text GLabel 7000 3500 0    50   Input ~ 0
EN
Wire Wire Line
	7000 2800 7150 2800
Wire Wire Line
	7000 2900 7150 2900
Wire Wire Line
	7000 3000 7150 3000
Wire Wire Line
	7000 3100 7150 3100
Wire Wire Line
	7000 3200 7150 3200
Wire Wire Line
	7000 3300 7150 3300
Wire Wire Line
	7000 3400 7150 3400
Wire Wire Line
	7000 3500 7150 3500
NoConn ~ 5100 2450
NoConn ~ 5200 2450
NoConn ~ 5300 2450
Text GLabel 6100 2300 1    50   Input ~ 0
+3V
Text GLabel 5900 2300 1    50   Input ~ 0
GND
Text GLabel 4700 4250 3    50   Input ~ 0
SDA
Text GLabel 4800 4250 3    50   Input ~ 0
SCL
Text GLabel 5200 4250 3    50   Input ~ 0
GPIO10
Text GLabel 5300 4250 3    50   Input ~ 0
GPIO11
Text GLabel 5400 4250 3    50   Input ~ 0
GPIO12
Text GLabel 5700 4250 3    50   Input ~ 0
EN
Wire Wire Line
	4700 4150 4700 4250
Wire Wire Line
	4800 4150 4800 4250
Wire Wire Line
	5200 4150 5200 4250
Wire Wire Line
	5300 4150 5300 4250
Wire Wire Line
	5400 4150 5400 4250
Wire Wire Line
	5700 4150 5700 4250
Wire Wire Line
	5900 2300 5900 2450
Wire Wire Line
	6100 2300 6100 2450
$EndSCHEMATC
