#include "EmonLib.h"             // Include Emon Library
EnergyMonitor emon1;             // Create an instance

void setup(){  
  Serial.begin(9600);
  emon1.voltage(2, 205, 1.7);  // Voltage: input pin, calibration, phase_shift
}

void loop(){
  emon1.calcVI(20,2000);           // Calculate all. No.of half wavelengths (crossings), time-out
  float Vrms = emon1.Vrms;
  Serial.print("Vrms = ");
  Serial.println(Vrms);
  delay(1000);
  //emon1.serialprint();           // Print out all variables (realpower, apparent power, Vrms, Irms, power factor)
}
