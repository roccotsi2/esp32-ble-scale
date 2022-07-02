#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include "datatypes.h"
#include <HX711.h>
#include "soc/rtc.h"
#include <EEPROM.h>

#define SERVICE_UUID "df469ada-185e-11ec-9621-0242ac130002"
#define CHARACTERISTIC_UUID "e8f1aaac-185e-11ec-9621-0242ac130002"

#define calibration_factor -24.93 // 7.235 //This value is obtained using the SparkFun_HX711_Calibration sketch
#define offset 37549

#define LOADCELL_DOUT_PIN  33
#define LOADCELL_SCK_PIN  32

const int8_t DEMO_MODE = 0;  // 1: demo mode on, 0: demo mode off
const int16_t EEPROM_ADDRESS = 0;

// HX711 variables
HX711 scale;
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
    
  }
}
