
/*
 * ESP8266 HTTPCLient Code V1.0 TESTED
 * Copyright (C) 2018 Samsoedien - https://github.com/samsoedien/ESP8266-data-collector
 *
 * Description: 
 * A simple program to perform GET and POST HTTP request without authentication to a REST API created in express/Nodejs.
 * 
 * How to use:
 * STEP 1: Use ESP8266-data-collector.ino and install the libraries from the github or arduino library manager.
 * IMPORTANT: Install Version 5 of ArduinoJSON library as newest version is still in beta mode and require different syntax.
 * STEP 2: Connect your desired sensor(s) to ESP8266 and define to what pin(s) it is connected.
 * STEP 3: Put your group number in GROUP_NUMBER (1 = Weather group, 2 = Drowsiness group, 3 = Stress group).
 * STEP 4: Fill in SSID and Password of WiFi network you want to use.
 * STEP 5: Set the interval timer to the desired rate of performing HTTP Request (minimum of 3000 is recommended).
 * STEP 6: In the readSensorData() method put your code. (NOTE: You don't need to touch the HTTPPostRequest method).
 * STEP 7: Make sure you map your sensordata to the array in HTTPPostRequest(sensordata[0], sensordata[1], sensordata[2], sensordata[3]) if you dont use all arrays just leave them empty.
 * STEP 8: Upload the sketch to ESP8266.
 */

#define GROUP_NUMBER 1 // 1 = Weather group, 2 = Drowsiness group, 3 = Stress group

#include "ESP8266WiFi.h"       // Library to connect ESP8266 to WiFi network.
#include "ESP8266HTTPClient.h" // Library to perform HTTP Requests.
#include "ArduinoJson.h"       // Library to parse sensordata in JSON format.

#define ANALOG_PIN0 A0 // Defined pin for the analog reading.
// define more pins if needed.

// WiFi parameters to be configured
const char *ssid = "xxxxxxxxxx";     // The SSID of the connected network.
const char *password = "xxxxxxxxxx"; // The password of the connected network.

const char *httpEndpoint1 = "http://beautiful-data.herokuapp.com/api/users/1/weatherdata";
const char *httpEndpoint2 = "http://beautiful-data.herokuapp.com/api/users/1/drowsinessdata";
const char *httpEndpoint3 = "http://beautiful-data.herokuapp.com/api/users/1/heartdata";
const char *route;

// Timer variables
unsigned long previousMillis = 0;
int intervalTimer = 10000;

int sensorData[4];

void setup(void)
{
  Serial.begin(115200);
  Serial.println(); //Clear some garbage that may be printed to the serial console
  Serial.print("Connecting..");
  WiFi.begin(ssid, password); // Connect to WiFi

  // while wifi not connected yet, print '.'
  // then after it connected, get out of the loop
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  // Connected.
  Serial.println("Connection Successful!");

  // Access Point (SSID).
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // IP adres.
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  // Signal Strength.
  long rssi = WiFi.RSSI();
  Serial.print("Signal Strength (RSSI): ");
  Serial.print(rssi);
  Serial.println(" dBm");
  Serial.println("");
}

void loop()
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= intervalTimer)
  {
    HTTPPostRequest(sensorData[0], sensorData[1], sensorData[2], sensorData[3]);
    previousMillis = currentMillis;
  }
  sensorData[0] = readSensorData();
  delay(20);
}

int readSensorData()
{

  // RUN YOUR CODE HERE AND RETURN IT TO MOVE IT OUT OF THE LOCAL SCOPE...
  // In case of measuring more sensordata use multiple functions or put all sensordata in array and parse in loop().

  int val = analogRead(ANALOG_PIN0);
  return val;
}

void HTTPPostRequest(int val1, int val2, int val3, int val4)
{
  if (WiFi.status() == WL_CONNECTED)
  { //Check WiFi connection status

    StaticJsonBuffer<300> JSONbuffer; //Declaring static JSON buffer
    JsonObject &JSONencoder = JSONbuffer.createObject();

    switch (GROUP_NUMBER)
    {
    case 1:
      JSONencoder["temperature"] = val1;
      JSONencoder["humidity"] = val2;
      JSONencoder["lightDensity"] = val3;
      JSONencoder["sound"] = val4;
      route = httpEndpoint1;
      break;
    case 2:
      JSONencoder["correctAnswers"] = val1;
      JSONencoder["wrongAnswers"] = val2;
      JSONencoder["totalTime"] = val3;
      route = httpEndpoint2;
      break;
    case 3:
      JSONencoder["bpm"] = val1;
      JSONencoder["ibi"] = val2;
      route = httpEndpoint3;
      break;
    }

    char JSONmessageBuffer[300];
    JSONencoder.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
    Serial.println(JSONmessageBuffer);

    HTTPClient http; //Declare object of class HTTPClient

    http.begin(route);                                  //Specify request destination
    http.addHeader("Content-Type", "application/json"); //Specify content-type header

    int httpCode = http.POST(JSONmessageBuffer); //Send the request
    String payload = http.getString();           //Get the response payload

    Serial.println(httpCode); //Print HTTP return code
    Serial.println(payload);  //Print request response payload

    http.end(); //Close connection
  }
  else
  {
    Serial.println("Error in WiFi connection");
  }
}
