#include "EmonLib.h"            

EnergyMonitor emon;       
float Energy_total; //= EEPROM.get(100, Energy_total);	//get value from eeprom if electricity cut off

float EnergyCalc(float V, float I, float P, unsigned long time){
  if ((V>190) && (I>=0.05)){			  	//If voltage <100 or current error from 0 is 0.05 skip energy addition.
	float Energy_sample = 0;				//new energy at time sample
	Energy_sample = P * time/3600000;	//time in ms to hour 
	Energy_total += Energy_sample;			//add to previous energy
  }
  //EEPROM.put(100 , Energy_total);			//save value in EEPROM
  return Energy_total;
}

void printEmon(float Vrms,float Irms,float Power,float Power2, float PF){
  Serial.print("Voltage= ");
  Serial.print(Vrms);
  Serial.println(" V");
  Serial.print("Current= ");
  Serial.print(Irms);
  Serial.println(" A");
  Serial.print("Power= ");
  Serial.println(Power);
  Serial.print("PowerReal= ");
  Serial.print(Power2);
  Serial.println(" W");
  Serial.print("Power Factor= ");
  Serial.println(PF);
}

void setup()
{  
  Serial.begin(9600);
  emon.voltage(A0, 150, 1.7);   				//For arduino: Voltage 170, Current 5.75/ For ESP32(3.3 V Supply): Voltage 75.25, Current 2.6
  emon.current(A1, 5.2);       				//(input pin, calibration, Phase shift (for Voltage))
}

void loop()
{
  unsigned long time1 = millis();
  emon.calcVI(20,2000);                  
  float Power       = emon.apparentPower;
  float Power2       = emon.realPower;
  float Vrms  		= emon.Vrms; 
  float Irms        = emon.Irms;					//float Irms = emon1.calcIrms(1480);  //Calculate Irms only
  float PF			= emon.powerFactor;
  printEmon(Vrms, Irms, Power, Power2, PF);
  /*
	Other CODE
  */
  unsigned long sample_time = millis() - time1;				//time of sample in ms
  Serial.print("Energy= ");
  Serial.print(EnergyCalc(Vrms, Irms, Power, sample_time));
  Serial.println(" Wh");
  Serial.println();
  delay(3000);
}
