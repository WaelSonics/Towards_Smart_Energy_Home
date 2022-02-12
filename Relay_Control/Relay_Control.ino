#include <EmonLib.h>
#include <Arduino.h>

#define Relay_Pin 4

void setup() {
  pinMode(Relay_Pin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  digitalWrite(Relay_Pin, HIGH);
  Serial.print("ON!!!!\n");
  delay(1000);
  digitalWrite(Relay_Pin, LOW);
  Serial.print("OFF!!!!\n");
  delay(1000);
}
