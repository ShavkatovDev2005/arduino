#define IBUS_BUFFSIZE 32
uint8_t ibus[IBUS_BUFFSIZE];
uint8_t idx = 0;

void setup() {
  Serial.begin(115200); // i-BUS receiver shu portga ulangan bo‘lishi kerak
}

void loop() {
  while (Serial.available()) {
    uint8_t b = Serial.read();
    if (idx == 0 && b != 0x20) continue; // paket uzunligi
    ibus[idx++] = b;
    if (idx == IBUS_BUFFSIZE) {
      idx = 0;
      // Har kanal 2 baytdan -> 14 ta kanal bor
      for (int i = 0; i < 10; i++) {
        int ch = ibus[2*i+2] | (ibus[2*i+3] << 8);
        Serial.print("CH"); Serial.print(i+1);
        Serial.print(": "); Serial.print(ch);
        Serial.print("  ");
      }
      Serial.println();
    }
  }
}