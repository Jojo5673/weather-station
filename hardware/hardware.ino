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

//MQTT and NTP declarations

static const char* id             = "620172690";
static const char* pubtopic       = "620172690";                   
static const char* subtopic[]     = {"620172690_sub","/elet2415"};  
static const char* mqtt_server    = "www.yanacreations.com";            
static uint16_t mqtt_port         = 1883;

const char* ssid                  = "NOIR"; // Add your Wi-Fi ssid
const char* password              = "baconeggandcheese"; // Add your Wi-Fi password 

TaskHandle_t xMQTT_Connect          = NULL; 
TaskHandle_t xNTPHandle             = NULL;  
TaskHandle_t xLOOPHandle            = NULL;  
TaskHandle_t xUpdateHandle          = NULL;
TaskHandle_t xButtonCheckeHandle    = NULL; 

void callback(char* topic, byte* payload, unsigned int length);
void vUpdate(void* pvParameters);
void vButtonCheck(void* pvParameters);

#ifndef NTP_H
#include "NTP.h"
#endif

#ifndef MQTT_H
#include "mqtt.h"
#endif

int moisture;
float humidity;
float temperature;
float pressure;
float altitude;
float heat_index;
char t_unit = 'C';
char a_unit = 'A';
char p_unit = 'P';


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
    vTaskDelete(NULL); // delete the loop task entirely
}

void vUpdate(void* pvParameters) {
  configASSERT(((uint32_t)pvParameters) == 1);

  for (;;) {
      moisture     = soilPercent(analogRead(SOIL));
      humidity     = dht.readHumidity();
      temperature  = dht.readTemperature();
      pressure     = bmp.readPressure();
      altitude     = bmp.readAltitude(1013.25);
      heat_index   = dht.computeHeatIndex(temperature, humidity, false);

      display_values();
      if(mqtt.connected()){  // only publish when ready
        publish();
      }

      vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

void publish(){   
  if (!mqtt.connected()) {
    Serial.println("MQTT not connected, skipping publish");
    return;
  }

  char payload[1100];

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
    Serial.println("Publising payload: ");
    Serial.println(payload);
    
    res = mqtt.publish(pubtopic, payload);

    if(!res){
      res = false;
      throw false;
    }else{
      Serial.println(" - Successful");
    }
  }
  catch(...){
    Serial.printf("\nError (%d) >> Unable to publish message\n", res);
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.printf("\nMessage received : ( topic: %s ) \n",topic ); 
  char *received = new char[length + 1] {0}; 

  for (int i = 0; i < length; i++) { 
    received[i] = (char)payload[i];    
  }
  Serial.printf("Payload : %s \n",received);

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, received);  
  delete[] received;

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  const char* t = doc["temperature"] | "c";
  const char* a = doc["altitude"]    | "m";
  const char* p = doc["pressure"]    | "pa";
  
  if (strcasecmp(t, "f")   == 0) t_unit = 'F';
  if (strcasecmp(t, "c")   == 0) t_unit = 'C';
  if (strcasecmp(a, "m")   == 0) a_unit = 'M';
  if (strcasecmp(a, "ft")  == 0) a_unit = 'F';
  if (strcasecmp(p, "atm") == 0) p_unit = 'A';
  if (strcasecmp(p, "pa")  == 0) p_unit = 'P';


}

unsigned long getTimeStamp(void){
    time_t now;  
    time(&now);
    return now;
}

void draw_indicators(int wifi_x, int mqtt_x, int y){

  static bool lastWifi = true;
  static bool lastMqtt = true;

  bool wifiConnected = WiFi.status() == WL_CONNECTED;
  bool mqttConnected = mqtt.connected();

  // only redraw if state changed
  if(wifiConnected == lastWifi && mqttConnected == lastMqtt) return;

  lastWifi = wifiConnected;
  lastMqtt = mqttConnected;

  int wifi_y =  y + 4;
  uint16_t color = ILI9341_WHITE;

  //wifi symbol
  tft.fillTriangle(wifi_x-9, wifi_y-9, wifi_x+9, wifi_y-9, wifi_x, wifi_y+3, BLUE); //center mask
  tft.drawCircle(wifi_x, wifi_y, 9, color);
  tft.drawCircle(wifi_x, wifi_y, 8, color);
  tft.drawCircle(wifi_x, wifi_y, 5, color);
  tft.drawCircle(wifi_x, wifi_y, 4, color);
  tft.fillCircle(wifi_x, wifi_y, 1, color); // dot at bottom

  tft.fillTriangle(wifi_x+17, wifi_y+9, wifi_x-4, wifi_y+9, wifi_x+7, wifi_y-6, BLUE); // left mask
  tft.fillTriangle(wifi_x-18, wifi_y+9, wifi_x+3, wifi_y+9, wifi_x-7, wifi_y-6, BLUE); // right mask

  if(!wifiConnected){
    // line through it
    tft.drawLine(wifi_x-5, y-5, wifi_x+5, y+5, color);
  }

  uint16_t mqttColor = mqttConnected ? ILI9341_GREEN : ILI9341_RED;
  tft.fillCircle(mqtt_x, y, 5, mqttColor);
  tft.drawCircle(mqtt_x, y, 5, color);
    
}

void display_values(){
  draw_indicators(104, 129, 11);

  char buffer[20];
  tft.setTextColor(ILI9341_WHITE, BLUE);
  
  // temperature
  float disp_temp = (t_unit == 'F') ? celsiusToFahrenheit(temperature) : temperature;
  int intPart = (int)disp_temp;
  int decPart = (int)(fabs(disp_temp - intPart) * 10 + 0.5);

  sprintf(buffer, "%3d", intPart); // decimal part
  tft.setCursor(1, 77);
  tft.setTextSize(4);
  tft.print(buffer);

  sprintf(buffer, ".%1d", decPart); // decimal part
  tft.setCursor(68, 93);
  tft.setTextSize(2);
  tft.print(buffer);

  tft.setCursor(80, 75); //unit
  tft.print(t_unit);
  
  //humidity
  sprintf(buffer, "%5.1f%%", humidity);
  tft.setCursor(21, 57);
  tft.print(buffer);

  //heat index
  float disp_hi = (t_unit == 'F') ? celsiusToFahrenheit(heat_index) : heat_index;
  tft.setTextSize(1);
  tft.setCursor(7, 117);
  tft.print("HI");

  tft.setTextSize(2);
  sprintf(buffer, "%5.1f%c", disp_hi, t_unit);  
  tft.setCursor(18, 113);
  tft.print(buffer);

  //pressure
  tft.setTextSize(1);
  if (p_unit == 'A') {
    sprintf(buffer, "%.4fatm", pascalsToAtm(pressure));
  } else {
    sprintf(buffer, "%7dPa", (int)pressure);
  }
  tft.setCursor(166, 118);
  tft.print(buffer);

  //altitude
  float disp_alt = (a_unit == 'F') ? metersToFeet(altitude) : altitude;
  tft.fillRect(224, 95, 5, 69, ILI9341_WHITE);
  tft.fillTriangle(226, 67, 234, 98, 218, 98, ILI9341_WHITE);
  tft.fillTriangle(226, 193, 219, 162, 233, 161, ILI9341_WHITE);

  tft.setTextSize(2);
  sprintf(buffer, (a_unit == 'F') ? "%4dft" : "%5dm", (int)disp_alt);
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

float celsiusToFahrenheit(float celsius) {
    return (celsius * 9.0 / 5.0) + 32.0;
}

float metersToFeet(float meters) {
    return meters * 3.28084;
}

float pascalsToAtm(float pascals) {
    return pascals / 101325.0;
}
