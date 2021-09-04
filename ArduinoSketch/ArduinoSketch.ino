/*
  ArduinoMqttClient - WiFi Simple Sender
  This example connects to a MQTT broker and publishes a message to
  a topic once a second.
  The circuit:
  - Arduino MKR 1000, MKR 1010 or Uno WiFi Rev2 board
  This example code is in the public domain.
*/

#include <ArduinoMqttClient.h>
#if defined(ARDUINO_SAMD_MKRWIFI1010) || defined(ARDUINO_SAMD_NANO_33_IOT) || defined(ARDUINO_AVR_UNO_WIFI_REV2)
  #include <WiFiNINA.h>
#elif defined(ARDUINO_SAMD_MKR1000)
  #include <WiFi101.h>
#elif defined(ARDUINO_ESP8266_ESP12)
  #include <ESP8266WiFi.h>
#endif

#include "arduino_secrets.h"
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;    // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)

// To connect with SSL/TLS:
// 1) Change WiFiClient to WiFiSSLClient.
// 2) Change port value from 1883 to 8883.
// 3) Change broker value to a server with a known SSL/TLS root certificate 
//    flashed in the WiFi module.

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "192.168.2.15";
int        port     = 1883;
const char topic[]  = "Greenify/plant1n/moisture";

const long interval = 5000;
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;

const int numReadings = 10;
int moistureBuffer[numReadings];
int readingMoisture = 0;
float moisture;

void setup() {
  // setup connection
  connectCom();
}

void loop() {
  // check if the connection is still up and reconnect otherwise
  if (!mqttClient.connected()){connectCom();}
  
  // call poll() regularly to allow the library to send MQTT keep alives which
  // avoids being disconnected by the broker
  mqttClient.poll();

  // to avoid having delays in loop, we'll use the strategy from BlinkWithoutDelay
  // see: File -> Examples -> 02.Digital -> BlinkWithoutDelay for more info
  currentMillis = millis();
  
  if (currentMillis - previousMillis >= interval) {
    // save the last time a message was sent
    previousMillis = currentMillis;

    // get measurement and average out the values
    // read from the sensor:
    readingMoisture = map(analogRead(A0),450,670,100,0);
 
    // calculate the average:
    moisture = averageMeasure(readingMoisture, moistureBuffer, numReadings);

    // send message, the Print interface can be used to set the message contents
    mqttClient.beginMessage(topic);
    //mqttClient.print("hello ");
    mqttClient.print(moisture);
    mqttClient.endMessage();
  }
  else if (previousMillis > currentMillis){
    previousMillis = currentMillis;
  }
}

void connectCom(){
  // attempt to connect to WiFi network:
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // failed, retry
    delay(5000);
  }

  if (!mqttClient.connect(broker, port)) {
    delay(1000);
  }
}

float averageMeasure(int measurement, int *measurementBuffer, int bufferSize){
  float average = 0;
  int currentValueBuffer;

  // shift elements in buffer to make room for new measurement
  // add each element in the buffer to average
  for(int i=bufferSize-1;i>0;i--){
    measurementBuffer[i] = measurementBuffer[i-1];
    average += measurementBuffer[i];
  }

  // add latest measurement to buffer and average
  measurementBuffer[0] = measurement;
  average += measurement;

  // calculate and return average
  return average /= bufferSize;
}
