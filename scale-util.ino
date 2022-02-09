// define commands
#define CMD_READ_WEIGHT "READ"

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

bool scaleutilIsCommandReadWeight(const char *buffer, int size) {
  if (size < sizeof(CMD_READ_WEIGHT)) {
    return false;
  }
  return scaleutilArrayEquals(buffer, CMD_READ_WEIGHT, sizeof(CMD_READ_WEIGHT));
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
    // TODO: read from scale
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
