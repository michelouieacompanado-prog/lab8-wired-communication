// Lab 8 - Example 1: UART loopback
// Wiring: jumper GPIO17 (TX2) to GPIO16 (RX2)

#include <Arduino.h>

const int UART_RX = 16;  // Serial2 RX
const int UART_TX = 17;  // Serial2 TX

void setup() {
  Serial.begin(115200);                          // USB serial to the Serial Monitor
  Serial2.begin(9600, SERIAL_8N1, UART_RX, UART_TX);
}

void loop() {
  static unsigned long lastSend = 0;
  if (millis() - lastSend >= 1000) {
    lastSend = millis();
    Serial2.println("PING");
  }
  while (Serial2.available()) {
    Serial.write(Serial2.read());
  }
}
