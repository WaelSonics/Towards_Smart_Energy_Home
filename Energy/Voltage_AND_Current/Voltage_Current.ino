#include "EmonLib.h"            
EnergyMonitor emon1;            

void setup()
{  
  Serial.begin(9600);
  
  emon1.voltage(2, 234.26, 1.7);  
  emon1.current(1, 111.1);       
}

void loop()
{
  emon1.calcVI(20,2000);        
  emon1.serialprint();          
  
  float realPower       = emon1.realPower;        
  float apparentPower   = emon1.apparentPower;    
  float powerFActor     = emon1.powerFactor;      
  float supplyVoltage   = emon1.Vrms;            
  float Irms            = emon1.Irms;            
}
