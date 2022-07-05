#include <DHT.h>
#include <Adafruit_NeoPixel.h>
#include <PMS.h>
#include <SPI.h>
#include <TFT_22_ILI9225.h>
#include <SoftwareSerial.h>

#include "FreeMonoBold24pt7b.h"

#define TFT_RST 7
#define TFT_RS  8
#define TFT_CS  10
#define TFT_LED 9
#define TFT_SDI 11
#define TFT_CLK 13

#define FanPWM 12

#define NEO_PIN 6

#define DHT_PIN 4

#define BUZZER_PIN 5

SoftwareSerial pmsSerial(2, 3);
PMS pms(pmsSerial);
PMS::DATA data;

TFT_22_ILI9225 tft = TFT_22_ILI9225(TFT_RST, TFT_RS, TFT_CS, TFT_LED, 100);

uint16_t x, y;
boolean flag = false;

uint16_t pm_color[] = {COLOR_BLACK, COLOR_NAVY, COLOR_GREEN, COLOR_ORANGE, COLOR_RED};
String pm_text[] = {"Waiting..", "Very Good", "Good", "Bad", "Very Bad"};
int8_t pm25_status = 0;
int8_t pm100_status = 0;
uint16_t pm25_color = pm_color[0];
uint16_t pm100_color = pm_color[0];
String pm25_text = "Setup!";
String pm100_text = "Booting...";
float temp;
float humi;


Adafruit_NeoPixel neoPixel = Adafruit_NeoPixel(1, NEO_PIN, NEO_GRB + NEO_KHZ800);
uint32_t neo_color[] = {neoPixel.Color(0, 0, 0), neoPixel.Color(0, 0, 128), neoPixel.Color(0, 255, 0), neoPixel.Color(255, 165, 0), neoPixel.Color(255, 0, 0)};

DHT dht(DHT_PIN, DHT11);

void DHT_Test() {
  Serial.begin(9600);
  Serial.println("DHT11 TEST PROGRAM");
  
  dht.begin();  
  delay(2000);  
  
  float h = dht.readHumidity(); 
  // Read temperature as Celsius (the default) 
  float t = dht.readTemperature(); 
  // Read temperature as Fahrenheit (isFahrenheit = true) 
  float f = dht.readTemperature(true); 
 
  // Check if any reads failed and exit early (to try again). 
  if (isnan(h) || isnan(t) || isnan(f)) { 
    Serial.println("Failed to read from DHT sensor!"); 
    
    return; 
  } 

  
}

void setup() {
  Serial.begin(9600);

  pmsSerial.begin(9600);

  dht.begin();


  pinMode(FanPWM,OUTPUT);

  tft.begin();
  tft.setOrientation(1);

  tft.setFont(Terminal6x8);
  tft.setBackgroundColor(COLOR_BLACK);
  tft.drawText(5 + 100 - 72, 6, "Temp:      `C", COLOR_WHITE);
  tft.drawText(5 + 100 + 10, 6, "Humi:      %", COLOR_WHITE);

  tft.fillRectangle(5, 20 + 2, 5 + 100, 20 + 2 + 74, pm_color[0]);
  tft.fillRectangle(5 + 100 + 10 + 0, 20 + 2, 5 + 100 + 10 + 15, 20 + 2 + 22, pm_color[1]);
  tft.fillRectangle(5 + 100 + 10 + 16, 20 + 2, 5 + 100 + 10 + 35, 20 + 2 + 22, pm_color[2]);
  tft.fillRectangle(5 + 100 + 10 + 36, 20 + 2, 5 + 100 + 10 + 75, 20 + 2 + 22, pm_color[3]);
  tft.fillRectangle(5 + 100 + 10 + 76, 20 + 2, 5 + 100 + 10 + 100, 20 + 2 + 22, pm_color[4]);

  tft.drawRectangle(5 + 100 + 10 + 0,  20 + 2 + 22 + 25, 5 + 100 + 10 + 100, 20 + 2 + 22 + 25 + 27, COLOR_WHITE);

  tft.fillRectangle(5, 20 + 78 + 2, 5 + 100, 20 + 78 + 2 + 74, pm_color[0]);
  tft.fillRectangle(5 + 100 + 10 + 0, 20 + 78 + 2, 5 + 100 + 10 + 15, 20 + 78 + 2 + 22, pm_color[1]);
  tft.fillRectangle(5 + 100 + 10 + 16, 20 + 78 + 2, 5 + 100 + 10 + 40, 20 + 78 + 2 + 22, pm_color[2]);
  tft.fillRectangle(5 + 100 + 10 + 41, 20 + 78 + 2, 5 + 100 + 10 + 75, 20 + 78 + 2 + 22, pm_color[3]);
  tft.fillRectangle(5 + 100 + 10 + 76, 20 + 78 + 2, 5 + 100 + 10 + 100, 20 + 78 + 2 + 22, pm_color[4]);

  tft.drawRectangle(5 + 100 + 10 + 0, 20 + 78 + 2 + 22 + 25, 5 + 100 + 10 + 100,  20 + 78 + 2 + 22 + 25 + 27, COLOR_WHITE);


  tft.setFont(Terminal11x16);
  tft.setBackgroundColor(pm25_color);
  tft.drawText(5 + 2, 20 + 2 + 2, "pm 2.5", COLOR_WHITE);
  tft.drawText(5 + 2, 20 + 78 + 2 + 2, "pm 10.0", COLOR_WHITE);
  tft.setGFXFont(&FreeMonoBold24pt7b);
  tft.drawGFXText(10, 20 + 2 + 74 - 10, "000", COLOR_WHITE);
  tft.drawGFXText(10, 20 + 78 + 2 + 74 - 10, "000", COLOR_WHITE);

  tft.setFont(Terminal11x16);
  tft.setBackgroundColor(COLOR_BLACK);
  tft.drawText(5 + 100 + 10 + 3,   20 + 2 + 22 + 25 + 6, pm25_text, COLOR_WHITE);
  tft.drawText(5 + 100 + 10 + 3,  20 + 78 + 2 + 22 + 25 + 6, pm100_text, COLOR_WHITE);

  neoPixel.begin();
  neoPixel.show();

  pinMode(BUZZER_PIN, OUTPUT);
}

void loop() {
  int light = analogRead(A0);
  int brightness = map(light, 0, 1024, 255, 0);

  if (pms.read(data)) {
    tft.setBacklightBrightness(brightness);
    neoPixel.setBrightness(brightness);

    bool temp_change, humi_change;
    temp_change = temp != dht.readTemperature();
    humi_change = humi != dht.readHumidity();

    tft.setFont(Terminal6x8);
    tft.setBackgroundColor(COLOR_BLACK);
    if (temp_change) {
      temp = dht.readTemperature();
      tft.fillRectangle(5 + 100 - 72 + 33 + 2, 6, 5 + 100 - 69 + 33 + 21, 6 + 8, COLOR_BLACK);
      if ((int)temp < 10) {
        tft.drawText(5 + 100 - 72 + 33 + 2 + 6, 6, String(temp, 1), COLOR_WHITE);
      } else {
        tft.drawText(5 + 100 - 72 + 33 + 2, 6, String(temp, 1), COLOR_WHITE);
      }
    }
    if (humi_change) {
      humi = dht.readHumidity();
      tft.fillRectangle(5 + 100 + 10 + 33, 6, 5 + 100 + 10 + 33 + 21, 6 + 8, COLOR_BLACK);
      if ((int)humi < 10) {
        tft.drawText(5 + 100 + 10 + 33 + 6, 6, String(humi, 1), COLOR_WHITE);
      } else {
        tft.drawText(5 + 100 + 10 + 33, 6, String(humi, 1), COLOR_WHITE);
      }
    }

    Serial.print("Temperature: ");
    Serial.print(temp);
    Serial.println(" ℃");
    Serial.print("humidity   : ");
    Serial.print(humi);
    Serial.println(" %");
    Serial.print("PM2.5      : ");
    Serial.print(data.PM_AE_UG_2_5);
    Serial.println(" ug/m3");
    Serial.print("pm10.0     : ");
    Serial.print(data.PM_AE_UG_10_0);
    Serial.println(" ug/m3");
    Serial.println("");

    bool pm25_change, pm100_change;
    if (0 == (int) data.PM_AE_UG_2_5) {
      pm25_change = pm25_status != 0;
      pm25_status = 0;
    }
    if (0 < (int) data.PM_AE_UG_2_5 && (int) data.PM_AE_UG_2_5 < 16) {
      pm25_change = pm25_status != 1;
      pm25_status = 1;

       pinMode(FanPWM,HIGH);
    }
    if (15 < (int) data.PM_AE_UG_2_5 && (int) data.PM_AE_UG_2_5 < 36) {
      pm25_change = pm25_status != 2;
      pm25_status = 2;

       pinMode(FanPWM,HIGH);
    }
    if (36 < (int) data.PM_AE_UG_2_5 && (int) data.PM_AE_UG_2_5 < 76) {
      pm25_change = pm25_status != 3;
      pm25_status = 3;

       pinMode(FanPWM,HIGH);
    }
    if (75 < (int) data.PM_AE_UG_2_5) {
      pm25_change = pm25_status != 4;
      pm25_status = 4;

      pinMode(FanPWM,HIGH);
    }
    if (0 == (int) data.PM_AE_UG_10_0) {
      pm100_change = pm100_status != 0;
      pm100_status = 0;
    }
    if (0 < (int) data.PM_AE_UG_10_0 && (int) data.PM_AE_UG_10_0 < 31) {
      pm100_change = pm100_status != 1;
      pm100_status = 1;

      pinMode(FanPWM,HIGH);
    }
    if (30 < (int) data.PM_AE_UG_10_0 && (int) data.PM_AE_UG_10_0 < 81) {
      pm100_change = pm100_status != 2;
      pm100_status = 2;

      pinMode(FanPWM,HIGH);
    }
    if (80 < (int) data.PM_AE_UG_10_0 && (int) data.PM_AE_UG_10_0 < 151) {
      pm100_change = pm100_status != 3;
      pm100_status = 3;

      pinMode(FanPWM,HIGH);
    }
    if (150 < (int) data.PM_AE_UG_10_0) {
      pm100_change = pm100_status != 4;
      pm100_status = 4;

     pinMode(FanPWM,HIGH);
    }
    pm25_color = pm_color[pm25_status];
    pm25_text = pm_text[pm25_status];
    pm100_color = pm_color[pm100_status];
    pm100_text = pm_text[pm100_status];


    int neo_status = max(pm25_status, pm100_status);
    for (uint16_t i = 0; i < neoPixel.numPixels(); i++) {
      neoPixel.setPixelColor(i, neo_color[neo_status]);
    }
    neoPixel.show();
    if (pm25_change || pm100_change) {
      if (neo_status == 4) {
        tone(BUZZER_PIN, 200);
      } else {
        noTone(BUZZER_PIN);
      }
    }

    int16_t w, h;
    tft.setGFXFont(&FreeMonoBold24pt7b);
    tft.setBackgroundColor(pm25_color);
    if (pm25_change) {
      tft.fillRectangle(5, 20 + 2, 5 + 100, 20 + 2 + 74, pm25_color);
      tft.setFont(Terminal11x16);
      tft.drawText(5 + 2, 20 + 2 + 2, "pm 2.5", COLOR_WHITE);
      tft.setBackgroundColor(COLOR_BLACK);
      tft.fillRectangle(5 + 100 + 10 + 0 + 1,  20 + 2 + 22 + 25 + 1, 5 + 100 + 10 + 100 - 1, 20 + 2 + 22 + 25 + 27 - 1, COLOR_BLACK);
      tft.drawText(5 + 100 + 10 + 0 + (100 - tft.getTextWidth(pm25_text)) / 2, 20 + 2 + 22 + 25 + 6, pm25_text, COLOR_WHITE);
    }
    tft.getGFXTextExtent(String(constrain((int) data.PM_AE_UG_2_5, 0, 999)), 0, 0, &w, &h);
    tft.fillRectangle(10, 20 + 2 + 74 - 10 - 30, 10 + 28 * 3+1, 20 + 2 + 74 - 10 + 1, pm25_color);
    if ((int) data.PM_AE_UG_2_5 != 0) {
      tft.drawGFXText(5 + (100 - w) / 2, 20 + 2 + 74 - 10, String(constrain((int) data.PM_AE_UG_2_5, 0, 999)), COLOR_WHITE);
    } else {
      tft.drawGFXText(5 + ( 100 - w) / 2, 20 + 2 + 74 - 10, "-", COLOR_WHITE);
    }

    tft.setBackgroundColor(pm100_color);
    if (pm100_change) {
      tft.fillRectangle(5, 20 + 78 + 2, 5 + 100, 20 + 78 + 2 + 74, pm100_color);
      tft.setFont(Terminal11x16);
      tft.drawText(5 + 2,  20 + 78 + 2 + 2, "pm 10.0", COLOR_WHITE);
      tft.setBackgroundColor(COLOR_BLACK);
      tft.fillRectangle(5 + 100 + 10 + 0 + 1, 20 + 78 + 2 + 22 + 25 + 1, 5 + 100 + 10 + 100 - 1, 20 + 78 + 2 + 22 + 25 + 27 - 1, COLOR_BLACK);
      tft.drawText(5 + 100 + 10 + 0 + (100 - tft.getTextWidth(pm100_text)) / 2, 20 + 78 + 2 + 22 + 25 + 6, pm100_text, COLOR_WHITE);
    }
    tft.getGFXTextExtent(String(constrain((int) data.PM_AE_UG_10_0, 0, 999)), 0, 0, &w, &h);
    tft.fillRectangle(10, 20 + 78 + 2 + 74 - 10 - 30, 10 + 28 * 3+1 , 20 + 78 + 2 + 74 - 10 + 1, pm100_color);
    if ((int) data.PM_AE_UG_10_0 != 0) {
      tft.drawGFXText(5 + (100 - w) / 2, 20 + 78 + 2 + 74 - 10, String(constrain((int) data.PM_AE_UG_10_0, 0, 999)), COLOR_WHITE);
    } else {
      tft.drawGFXText(5 + (100 - w) / 2, 20 + 78 + 2 + 74 - 10, "-", COLOR_WHITE);
    }


    int16_t x;
    tft.fillRectangle(5 + 100 + 10 + 0 - 5,  20 + 2 + 22 + 1, 5 + 100 + 10 + 100 + 5, 20 + 2 + 22 + 24, COLOR_BLACK);
    x = map(min((int) data.PM_AE_UG_2_5, 100), 0, 100, 0, 100);
    if ((int) data.PM_AE_UG_2_5 != 0) {
      tft.fillTriangle(5 + 100 + 10 + 0 + 0 + x,  20 + 2 + 22 + 1 + 0, 5 + 100 + 10 + 0 + 5 + x,  20 + 2 + 22 + 10, 5 + 100 + 10 + 0 - 5 + x,  20 + 2 + 22 + 10, COLOR_WHITE);
      tft.drawLine(5 + 100 + 10 + 0 + 0 + x,  20 + 2 + 22 + 1, 5 + 100 + 10 + 0 + 0 + x,  20 + 2 + 22 + 24, COLOR_WHITE);
    }
    tft.fillRectangle(5 + 100 + 10 + 0 - 5, 20 + 78 + 2 + 22 + 1, 5 + 100 + 10 + 100 + 5, 20 + 78 + 2 + 22 + 24, COLOR_BLACK);
    x = map(min((int) data.PM_AE_UG_10_0, 200), 0, 200, 0, 100);
    if ((int) data.PM_AE_UG_10_0 != 0) {
      tft.fillTriangle(5 + 100 + 10 + 0 + 0 + x, 20 + 78 + 2 + 22 + 1 + 0, 5 + 100 + 10 + 0 + 5 + x, 20 + 78 + 2 + 22 + 10, 5 + 100 + 10 + 0 - 5 + x, 20 + 78 + 2 + 22 + 10, COLOR_WHITE);
      tft.drawLine(5 + 100 + 10 + 0 + 0 + x, 20 + 78 + 2 + 22 + 1, 5 + 100 + 10 + 0 + 0 + x, 20 + 78 + 2 + 22 + 24, COLOR_WHITE);
    }
  }
}