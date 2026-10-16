// Lab 8 - Example 2: I2C address scanner
// Wiring: module SDA -> GPIO32, SCL (labeled SCK on the OLED) -> GPIO33, VDD -> 3V3, GND -> GND

#include <Wire.h>

const int I2C_SDA = 32;
const int I2C_SCL = 33;

void setup() {
  Serial.begin(115200);
  Wire.begin(I2C_SDA, I2C_SCL);

  int found = 0;
  for (uint8_t address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    if (Wire.endTransmission() == 0) {
      Serial.print("Found device at 0x");
      Serial.println(address, HEX);
      found++;
    }
  }
  if (found == 0) {
    Serial.println("No I2C devices found");
  }
}

void loop() {}
