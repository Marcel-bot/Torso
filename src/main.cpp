#include <main.h>

void setup() {
  Serial.begin(BAUDRATE);
  Wire.begin();
}

void loop() {
  if (Serial.available()) {
    Wire.beginTransmission(RIGHT_HAND_ADDRESS);
    Wire.write(Serial.read());
    Wire.endTransmission();
  }
}