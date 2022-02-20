//#pragma pack(1)
typedef struct scaleCurrentWeight {
   int16_t currentBruttoGram;
} __attribute__ ((packed)) ScaleCurrentWeight;

// struct for saving data in EEPROM
typedef struct scaleConfig {
  float tareOffset;
  float calibrationFactor;
} __attribute__ ((packed)) ScaleConfig;
