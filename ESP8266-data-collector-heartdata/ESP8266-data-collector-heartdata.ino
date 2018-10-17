
/*
 * ESP8266 HTTPCLient Code V1.0 TESTED
 * Copyright (C) 2018 Samsoedien - https://github.com/samsoedien/ESP8266-data-collector
 *
 * Description: 
 * A simple program to perform GET and POST HTTP request without authentication to a REST API created in express/Nodejs.
 * 
 * How to use:
 * STEP 1: Install the libraries from the github or arduino library manager.
 * IMPORTANT: Install Version 5 of ArduinoJSON library as newest version is still in beta mode and require different syntax.
 * STEP 2: Connect your desired sensor(s) to ESP8266 and define to what pin(s) it is connected.
 * STEP 3: Fill in SSID and Password of WiFi network you want to use.
 * STEP 4: Set the interval timer to the desired rate of performing HTTP Request (minimum of 3000 is recommended)
 * STEP 5: Uncomment the route to perform your HTTP requests.
 * STEP 6: In the loop() choose a variable to send to http endpoint by putting it in the method: HTTPPostRequest(exampleVariable);
 * STEP 7: Use JSONencoder attributes: JSONencoder["attribute"] with corresponding values.
 * STEP 8: Upload the sketch to ESP8266.
 */

#include "ESP8266WiFi.h"                  // Library to connect ESP8266 to WiFi network.
#include "ESP8266HTTPClient.h"            // Library to perform HTTP Requests.
#include "ArduinoJson.h"                  // Library to parse sensordata in JSON format.

#define USE_ARDUINO_INTERRUPTS false
#include <PulseSensorPlayground.h>

#define PULSE_PIN A0                     // Defined pin for the analog reading.

int sensordata;

// WiFi parameters to be configured
const char *ssid = "Samsoedien iPhone";   // The SSID of the connected network.
const char *password = "xagven4jwcdxx";   // The password of the connected network.

const char *httpEndpoint = "http://beautiful-data.herokuapp.com/api/users/1/heartdata";

//const char* route = "/weatherdata";
//const char* route = "/drowsydata";
const char *route = "/heartdata";

// Timer variables
unsigned long previousMillis = 0;
int intervalTimer = 10000;

const int OUTPUT_TYPE = SERIAL_PLOTTER;

const int PULSE_INPUT = A0;
const int PULSE_BLINK = LED_BUILTIN;    // Pin 13 is the on-board LED
const int PULSE_FADE = 5;
const int THRESHOLD = 550;   // Adjust this number to avoid noise when idle

byte samplesUntilReport;
const byte SAMPLES_PER_SERIAL_SAMPLE = 10;

int myBPM = 0;

PulseSensorPlayground pulseSensor;

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

  pulseSensor.analogInput(PULSE_INPUT);
  pulseSensor.blinkOnPulse(PULSE_BLINK);
  pulseSensor.fadeOnPulse(PULSE_FADE);

  pulseSensor.setSerial(Serial);
  pulseSensor.setOutputType(OUTPUT_TYPE);
  pulseSensor.setThreshold(THRESHOLD);

  // Skip the first SAMPLES_PER_SERIAL_SAMPLE in the loop().
  samplesUntilReport = SAMPLES_PER_SERIAL_SAMPLE;
}

void loop()
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= intervalTimer && sensordata != 0)
  {
    HTTPPostRequest(sensordata);
    previousMillis = currentMillis;
  }
  sensordata = heartData();  
  delay(20);
}

int heartData()
{
  if (pulseSensor.sawNewSample()) {
    if (pulseSensor.sawStartOfBeat()) { 
      myBPM = pulseSensor.getBeatsPerMinute(); 
      Serial.print("BPM: "); 
      Serial.println(myBPM); 
    }
  }
  return myBPM;
}

void HTTPPostRequest(int analogVal)
{
  if (WiFi.status() == WL_CONNECTED)
  { //Check WiFi connection status

    StaticJsonBuffer<300> JSONbuffer; //Declaring static JSON buffer
    JsonObject &JSONencoder = JSONbuffer.createObject();

    JSONencoder["bpm"] = analogVal; // specify your attribute name and the sensordata it needs to send.

    char JSONmessageBuffer[300];
    JSONencoder.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
    Serial.println(JSONmessageBuffer);

    HTTPClient http; //Declare object of class HTTPClient

    http.begin(httpEndpoint);                           //Specify request destination
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
