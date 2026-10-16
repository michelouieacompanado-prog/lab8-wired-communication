// Lab 8 - Example 3: SPI loopback
// Wiring: jumper GPIO13 (MOSI) to GPIO26 (MISO)

#include <SPI.h>

const int SPI_SCK  = 14;
const int SPI_MISO = 26;
const int SPI_MOSI = 13;
const int SPI_CS   = 4;

void setup() {
  Serial.begin(115200);
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI, SPI_CS);
  pinMode(SPI_CS, OUTPUT);
  digitalWrite(SPI_CS, HIGH);  // idle: slave deselected
}

void loop() {
  static unsigned long lastTransfer = 0;
  if (millis() - lastTransfer >= 1000) {
    lastTransfer = millis();

    SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
    digitalWrite(SPI_CS, LOW);
    uint8_t reply = SPI.transfer(0xA5);
    digitalWrite(SPI_CS, HIGH);
    SPI.endTransaction();

    Serial.print("Sent 0xA5, received 0x");
    Serial.println(reply, HEX);
  }
}
