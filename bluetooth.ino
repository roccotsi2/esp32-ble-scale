// define UUIDs for bluetooth
#define SERVICE_UUID           "df469ada-185e-11ec-9621-0242ac130002"
#define CHARACTERISTIC_UUID_RX "e8f1aaac-185e-11ec-9621-0242ac130002"
#define CHARACTERISTIC_UUID_TX "71c90414-1860-11ec-9621-0242ac130002"

// variables
BLECharacteristic *pCharacteristicWrite;
BLECharacteristic *pCharacteristicRead;
bool deviceConnected = false;

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      Serial.println("Connection established");
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      Serial.println("Connection lost");
      deviceConnected = false;

      // start advertising again
      pServer->getAdvertising()->start();
      Serial.println("Advertising started");
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();

      if (rxValue.length() > 0) {
        Serial.println("*********");
        Serial.print("Received Value: ");

        for (int i = 0; i < rxValue.length(); i++) {
          if (rxValue[i]<0x10) {
            Serial.print("0");
          }
          Serial.print(rxValue[i], HEX);
          Serial.print(' ');
        }

        Serial.println();

        if (scaleutilIsCommandReadWeight(rxValue.data(), sizeof(rxValue))) {
          scaleutilSendScaleCurrentWeight();
        } else if (scaleutilIsCommandTare) {
          scaleutilTare();
        }
      }  
    }
};

void bluetoothSetupServer() {
  // Create the BLE Device
  BLEDevice::init("SCALE-18626452735"); // Give it a name

  // Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristicWrite = pService->createCharacteristic(
                      CHARACTERISTIC_UUID_TX,
                      BLECharacteristic::PROPERTY_WRITE
                    );

  pCharacteristicWrite->setCallbacks(new MyCallbacks());

  pCharacteristicRead = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID_RX,
                                         BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
                                       );

  pCharacteristicRead->addDescriptor(new BLE2902());

  // Start the service
  pService->start();  

  // Start advertising
  pServer->getAdvertising()->addServiceUUID(BLEUUID(SERVICE_UUID));
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
}

void bluetoothSendByteArray(byte *buffer, int dataLength) {
  int sentBytes = 0;
  int chunkSize = BLEDevice::getMTU() - 3;
  byte tmpBuffer[chunkSize];
  while (sentBytes < dataLength) {
    int countBytes = (dataLength - sentBytes) > chunkSize ? chunkSize : (dataLength - sentBytes);
    for (int i = 0; i < countBytes; i++) {
      tmpBuffer[i] = buffer[sentBytes + i];
    }
    pCharacteristicRead->setValue(tmpBuffer, countBytes); 
    pCharacteristicRead->notify();
    sentBytes = sentBytes + countBytes;
  }
}

bool bluetoothIsDeviceConnected() {
  return deviceConnected;
}
