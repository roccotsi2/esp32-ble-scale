// define commands
#define CMD_READ_WEIGHT "READ"
#define CMD_TARE        "TARE"

// variables
ScaleCurrentWeight currentScaleCurrentWeight;

bool scaleutilArrayEquals(const char *buffer1, const char *buffer2, int size) {
  for (int i = 0; i < size; i++) {
    if (buffer1[i] != buffer2[i]) {
      return false;
    }
  }
  return true; // the arrays are equal up to index (size - 1)
}

// swap endians of int16_t
void scaleutilSwapBmsBytesEndian(byte *buffer, int size) {
  byte tmpValue;
  for (int i = 0; i < (size / 2); i++) {
    tmpValue = buffer[2*i];
    buffer[2*i] = buffer[2*i + 1];
    buffer[2*i + 1] = tmpValue;
  }
}

ScaleConfig scaleutilLoadScaleConfigFromEeprom() {
  ScaleConfig scaleConfigLocal;
  EEPROM.begin(512);
  EEPROM.get(EEPROM_ADDRESS, scaleConfigLocal);
  EEPROM.end();
  return scaleConfigLocal;
}

void scaleutilSaveScaleConfigToEeprom(ScaleConfig scaleConfigLocal) {
  EEPROM.begin(512);
  EEPROM.put(EEPROM_ADDRESS, scaleConfigLocal);
  EEPROM.commit();
  EEPROM.end();
}

void scaleutilInitializeLoadCell() {
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(calibration_factor); //This value is obtained by using the SparkFun_HX711_Calibration sketch
  scale.set_offset(offset);
}

bool scaleutilIsCommandReadWeight(const char *buffer, int size) {
  if (size < sizeof(CMD_READ_WEIGHT)) {
    return false;
  }
  return scaleutilArrayEquals(buffer, CMD_READ_WEIGHT, sizeof(CMD_READ_WEIGHT));
}

bool scaleutilIsCommandTare(const char *buffer, int size) {
  if (size < sizeof(CMD_TARE)) {
    return false;
  }
  return scaleutilArrayEquals(buffer, CMD_TARE, sizeof(CMD_TARE));
}

void scaleutilWriteScaleCurrentWeightToBuffer(byte *buffer, int size, ScaleCurrentWeight *scaleCurrentWeight) {
  if (size < sizeof(ScaleCurrentWeight)) {
    Serial.println("scaleutilWriteScaleCurrentWeightToBuffer: buffer too small");
    return;
  }
  memcpy(buffer, scaleCurrentWeight, sizeof(ScaleCurrentWeight));

  // swap bytes to little endian (as structs are organized in little endian in ESP32)
  scaleutilSwapBmsBytesEndian(buffer, sizeof(ScaleCurrentWeight));
}

// fills scaleCurrentWeight with current values
void scaleutilFillScaleCurrentWeight(ScaleCurrentWeight *scaleCurrentWeight) {
  if (DEMO_MODE == 1) {
    // simulation
    if (scaleCurrentWeight -> currentBruttoGram > 1001) {
      scaleCurrentWeight -> currentBruttoGram = scaleCurrentWeight -> currentBruttoGram - 1;
    } else {
      scaleCurrentWeight -> currentBruttoGram = 15000; // initial demo value
    }
  } else {
    // HX711
    float valueGram = scale.get_units(20);
    Serial.print("Raw Value: ");
    Serial.println(valueGram);
    scaleCurrentWeight -> currentBruttoGram = round(valueGram);
    Serial.print("Rounded Value: ");
    Serial.println(scaleCurrentWeight -> currentBruttoGram);
  }
}

void scaleutilTare() {
  if (DEMO_MODE == 0) {
    Serial.println("Taring...");
    scale.tare(); 
    //LoadCell.start(2000); // 2 sec for taring
    scaleConfig.tareOffset = scale.get_offset();;
    Serial.print("Tare Offset = ");
    Serial.println(scaleConfig.tareOffset);
    scaleutilSaveScaleConfigToEeprom(scaleConfig);
    Serial.println("Taring finished and written to EEPROM");
    scaleutilInitializeLoadCell();
  } else {
    // DEMO MODE: set current Weight to default value
    currentScaleCurrentWeight.currentBruttoGram = 15000; // initial demo value
  }
}

void scaleutilSendScaleCurrentWeight() {
  Serial.println("Sending ScaleCurrentWeight");  
  byte bufferWeight[sizeof(ScaleCurrentWeight)];
  scaleutilFillScaleCurrentWeight(&currentScaleCurrentWeight);
  Serial.print("Value: ");
  Serial.println(currentScaleCurrentWeight.currentBruttoGram);
  scaleutilWriteScaleCurrentWeightToBuffer(bufferWeight, sizeof(bufferWeight), &currentScaleCurrentWeight);
  bluetoothSendByteArray(bufferWeight, sizeof(bufferWeight));
}
