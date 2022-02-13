#include "EmonLib.h"             
EnergyMonitor emon1;             

void setup(){  
  Serial.begin(9600);
  pinMode(34, INPUT);
  emon1.voltage(34, 60, 1.7);  // Voltage: input pin, calibration(205 with arduino, 60 with ESP32 and devider, 41 without devider), phase_shift
}

void loop(){
  emon1.calcVI(20,2000);           // Calculate all. No.of half wavelengths, time-out
  float Vrms = emon1.Vrms;
  Serial.print("Vrms = ");
  Serial.println(Vrms);
  delay(1000);
}
