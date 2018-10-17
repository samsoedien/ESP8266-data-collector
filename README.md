# ESP8266-data-collector

ESP8266 HTTPClient Data Collector Code V1.1 TESTED
Copyright (C) 2018 Samsoedien

## Description:

A simple program to perform GET and POST HTTP request without authentication to a REST API created in express/Nodejs.

## How to use:

STEP 1: Use ESP8266-data-collector.ino and install the libraries from the github or arduino library manager.

**IMPORTANT:** Install Version 5 of ArduinoJSON library as newest version is still in beta mode and require different syntax.

STEP 2: Connect your desired sensor(s) to ESP8266 and define to what pin(s) it is connected.

STEP 3: Put your group number in GROUP_NUMBER (1 = Weather group, 2 = Drowsiness group, 3 = Stress group).

STEP 4: Fill in SSID and Password of WiFi network you want to use.

STEP 5: Set the interval timer to the desired rate of performing HTTP Request (minimum of 3000 is recommended).

STEP 6: In the readSensorData() method put your code. (NOTE: You don't need to touch the HTTPPostRequest method).

STEP 7: Make sure you map your sensordata to the array in HTTPPostRequest(sensordata[0], sensordata[1], sensordata[2], sensordata[3]) if you dont use all arrays just leave them empty.

STEP 8: Upload the sketch to ESP8266.
