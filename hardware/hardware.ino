#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>
#include <DHT.h>
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <rom/rtc.h>
#include "screen.h"

#ifndef _WIFI_H 
#include <WiFi.h>
#endif

#ifndef STDLIB_H
#include <stdlib.h>
#endif

#ifndef STDIO_H
#include <stdio.h>
#endif

#ifndef ARDUINO_H
#include <Arduino.h>
#endif 
 
#ifndef ARDUINOJSON_H
#include <ArduinoJson.h>
#endif

#ifndef NTP_H
#include "NTP.h"
#endif

#ifndef MQTT_H
#include "mqtt.h"
#endif

#define TFT_DC    17
#define TFT_CS    5
#define TFT_RST   16
#define TFT_CLK   18
#define TFT_MOSI  23
#define TFT_MISO  19

#define BMP_SCL 33
#define BMP_SDA 25

#define DHTPIN 26
#define DHTTYPE DHT22

#define SOIL 32

#define BLUE 0x563d
#define BROWN 0x9328

static const char* id             = "620172690";
static const char* pubtopic       = "620172690";                   
static const char* subtopic[]     = {"620172690_sub","/elet2415"};  
static const char* mqtt_server    = "www.yanacreations.com";            
static uint16_t mqtt_port         = 1883;

const char* ssid                  = "One"; // Add your Wi-Fi ssid
const char* password              = "g5dnTphrhqpw"; // Add your Wi-Fi password 

TaskHandle_t xMQTT_Connect          = NULL; 
TaskHandle_t xNTPHandle             = NULL;  
TaskHandle_t xLOOPHandle            = NULL;  
TaskHandle_t xUpdateHandle          = NULL;
TaskHandle_t xButtonCheckeHandle    = NULL; 

int moisture;
float humidity;
float temperature;
float pressure;
float altitude;
float heat_index;

void callback(char* topic, byte* payload, unsigned int length);

Adafruit_BMP280 bmp;
DHT dht(DHTPIN, DHTTYPE);
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

void setup() {
  Serial.begin(115200); // serial port setup
  Wire.begin(BMP_SDA, BMP_SCL);
  dht.begin();

  Serial.println(F("BMP280 test"));
  unsigned status;

  status = bmp.begin(0x76);
  if (!status) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
                      "try a different address!"));
    Serial.print("SensorID was: 0x"); Serial.println(bmp.sensorID(),16);
    Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
    Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
    Serial.print("        ID of 0x60 represents a BME 280.\n");
    Serial.print("        ID of 0x61 represents a BME 680.\n");
    while (1) delay(10);
  }

  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

  tft.begin();
  tft.setRotation(2);
  tft.drawRGBBitmap(0,0, screen, 240, 320); // DRAW IMAGE ON SCREEN
  tft.setTextSize(2);

  initialize(); 
}

void loop() {
  moisture = soilPercent(analogRead(SOIL));
  Serial.print("Soil Moisture Percent: ");
  Serial.print(moisture); // read sensor
  Serial.println(" %"); 

  humidity = dht.readHumidity();
  Serial.print(F("Humidity = "));
  Serial.print(humidity);
  Serial.println(" %");

  temperature = dht.readTemperature();
  Serial.print(F("Temperature = "));
  Serial.print(temperature);
  Serial.println(" *C");

  pressure = bmp.readPressure();
  Serial.print(F("Pressure = "));
  Serial.print(pressure);
  Serial.println(" Pa");

  altitude = bmp.readAltitude(1013.25);
  Serial.print(F("Approx altitude = "));
  Serial.print(altitude); 
  Serial.println(" m");

  heat_index = dht.computeHeatIndex(temperature, humidity, false);;
  Serial.print(F("Heat index "));
  Serial.print(heat_index); 
  Serial.println(" *C");

  display_values();
  publish();
  Serial.println();
  vTaskDelay(2000 / portTICK_PERIOD_MS);  
}

void publish(){   
  char* payload;

  JsonDocument doc; // Create JSon object
  doc["id"]           = id; 
  doc["timestamp"]    = getTimeStamp();
  doc["moisture"]     = moisture;
  doc["humidity"]     = humidity;
  doc["temperature"]  = temperature;
  doc["pressure"]     = pressure;
  doc["altitude"]     = altitude;
  doc["heat_index"]   = heat_index;

  serializeJson(doc, payload);

  bool res = false;
  try{
    Serial.printf("Publising payload: ", payload);
    
    res = mqtt.publish(pubtopic, payload);

    if(!res){
      res = false;
      throw false;
    }else{
      Serial.print(" - Successful")
    }
  }
  catch(...){
    Serial.printf("\nError (%d) >> Unable to publish message\n", res);
  }
  return res;
}

void callback(char* topic, byte* payload, unsigned int length) {
  // ############## MQTT CALLBACK  ######################################
  // RUNS WHENEVER A MESSAGE IS RECEIVED ON A TOPIC SUBSCRIBED TO
  
  Serial.printf("\nMessage received : ( topic: %s ) \n",topic ); 
  char *received = new char[length + 1] {0}; 
  
  for (int i = 0; i < length; i++) { 
    received[i] = (char)payload[i];    
  }

  Serial.printf("Payload : %s \n",received);
}

void display_values(){
  char buffer[20];
  tft.setTextColor(ILI9341_WHITE, BLUE);
  
  // temperature
  int intPart = (int)temperature;
  int decPart = (int)(fabs(temperature - intPart) * 10 + 0.5); // 1 decimal place

  sprintf(buffer, "%3d", intPart); // decimal part
  tft.setCursor(1, 77);
  tft.setTextSize(4);
  tft.print(buffer);

  sprintf(buffer, ".%1d", decPart); // decimal part
  tft.setCursor(68, 93);
  tft.setTextSize(2);
  tft.print(buffer);

  tft.setCursor(80, 75); //unit
  tft.print("C");
  
  //humidity
  sprintf(buffer, "%5.1f%%", humidity);
  tft.setCursor(21, 57);
  tft.print(buffer);

  //heat index
  tft.setTextSize(1);
  tft.setCursor(7, 117);
  tft.print("HI");

  tft.setTextSize(2);
  sprintf(buffer, "%5.1fC", heat_index);
  tft.setCursor(18, 113);
  tft.print(buffer);

  //pressure
  tft.setTextSize(1);
  sprintf(buffer, "%6dPa", (int)pressure);
  tft.setCursor(174, 118);
  tft.print(buffer);

  //altitude
  tft.fillRect(224, 95, 5, 69, ILI9341_WHITE);
  tft.fillTriangle(226, 67, 234, 98, 218, 98, ILI9341_WHITE);
  tft.fillTriangle(226, 193, 219, 162, 233, 161, ILI9341_WHITE);

  tft.setTextSize(2);
  sprintf(buffer, "%5dm", (int)altitude);
  tft.setCursor(150, 129);
  tft.print(buffer);

  // moisture
  sprintf(buffer, "%3d%%", moisture);   // fixed width

  tft.setTextColor(ILI9341_WHITE, BROWN);
  tft.setCursor(110, 297);
  tft.print(buffer);
}

int soilPercent(int value){
    int percent = map(value, 2600, 550, 0, 100);
    return constrain(percent, 0, 100);
}
