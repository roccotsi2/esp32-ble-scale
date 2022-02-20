#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include "datatypes.h"
#include <HX711_ADC.h>
#include "soc/rtc.h"
#include <EEPROM.h>

#define SERVICE_UUID        "df469ada-185e-11ec-9621-0242ac130002"
#define CHARACTERISTIC_UUID "e8f1aaac-185e-11ec-9621-0242ac130002"

const int8_t DEMO_MODE = 0; // 1: demo mode on, 0: demo mode off
const int16_t EEPROM_ADDRESS = 0;
const float INITIAL_TARE_OFFSET = 8424208;
const float INITIAL_CALIBRATION_FACTOR = -4.13;

// HX711 variables
HX711_ADC LoadCell(33, 32); // HX711 constructor (dout pin, sck pin)
ScaleConfig scaleConfig;

long t;

void setup() {
  // reduce the CPU speed, because ESP32 is too fast for HX711
  rtc_clk_cpu_freq_set(RTC_CPU_FREQ_80M);
  
  Serial.begin(115200);

  if (DEMO_MODE == 0) {
    // no demo mode -> initialze the scale
    scaleutilInitializeLoadCell();
  }
  
  bluetoothSetupServer();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (DEMO_MODE == 0) {
    //update() should be called at least as often as HX711 sample rate; >10Hz@10SPS, >80Hz@80SPS
    LoadCell.update();

    //get smoothed value from the data set
    if (millis() > t + 1000) {
      float i = LoadCell.getData();
      Serial.print("Load_cell output val: ");
      Serial.println(i);
      t = millis();
    }
  }
}
