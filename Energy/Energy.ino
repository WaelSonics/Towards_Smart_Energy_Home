#include "EmonLib.h"            
EnergyMonitor emon1;       

float Energy_total;

float EnergyCalc(float V, float I, float P, unsigned long time){
	if ((V>100) && (I>=0.05)){			  		//If voltage <100 or current error from 0 is 0.05 skip energy addition.
		float Energy_sample = 0;				    //new energy at time sample
		Energy_sample = P * time/3600000;		//time in ms to hour
		Energy_total += Energy_sample;			//add to previous energy
	}
	return Energy_total;
}	

void setup()
{  
  Serial.begin(9600);
  emon1.voltage(34, 75.3, 1.7);   //For arduino: Voltage 170, Current 5.75/ For ESP32(3.3 V Supply): Voltage 75.25, Current 2.6
  emon1.current(35, 5.75);       
}

void loop()
{
  unsigned long time1 = millis();
  emon1.calcVI(20,2000);                  
  float realPower       = emon1.realPower;        
  float Vrms  		      = emon1.Vrms;            
  float Irms            = emon1.Irms; 
  Serial.print("Real Power= ");
  Serial.println(realPower);
  Serial.print("Voltage= ");
  Serial.println(Vrms);
  Serial.print("Current= ");
  Serial.println(Irms);
  Serial.println();
  delay(1000);
  unsigned long sample_time = millis() - time1;						//time of sample in ms
  EnergyCalc(Vrms, Irms, realPower, sample_time);     		//final data needed
}
