#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include "datatypes.h"

#define SERVICE_UUID        "df469ada-185e-11ec-9621-0242ac130002"
#define CHARACTERISTIC_UUID "e8f1aaac-185e-11ec-9621-0242ac130002"

const byte DEMO_MODE = 1; // 1: demo mode on, 0: demo mode off

void setup() {
  Serial.begin(115200);
  bluetoothSetupServer();
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(2000);
}
