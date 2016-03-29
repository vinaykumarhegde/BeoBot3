/* 
 * Arduino Nano 2 (URM04 Driver)
 * 
 * Author: Vinay (me@vnay.in)
 * 
 * Addresses:
 *      UR_L2N  - 0x11
 *      UR_L2NW - 0x12
 *      UR_L2NE - 0x13
 *      UR_L2S  - 0x14
 *      UR_L2SW - 0x15
 *      UR_L2SE - 0x16
 * 
 */

#undef DEBUG
 
#include "URM04Driver.h"


const uint8_t numSensors = 6; // Number of sensors
const byte initAdd = 0x11; // Initial address of sensor bunch.
URM04Sensor l[numSensors]; // URM04 sensors.
byte distances[2*numSensors]; // 2 bytes for each sensor distance data.

void setup(){
  Serial.begin(19200);
  Serial.println("Initalizing....");
//  setAdd(0x11);
  initURM(l,numSensors,initAdd);
}

void readURMDist(URM04Sensor l[], uint8_t num, byte dist[]){
  for(int i=0; i<num; ++i){
    l[i].Trigger();
    delay(TRIGGERDELAY);
    l[i].ReadSignal();
    delay(READDELAY);
    uint16_t di = l[i].ReadDistance();
    dist[i] = di/256; 
    dist[i+1] = di % 256; 
    Serial.print("Distance (sensor:");
    Serial.print(i); Serial.print(": ");
    Serial.println(di);
    clearRxBuffer();
  }
}

void loop(){
//  l[0].Trigger();
//  delay(30);// 30ms delay after trigger
//  l[0].ReadSignal();
//  delay(10); // 10ms delay after read signal
//  uint16_t dist = l[0].ReadDistance();
//  Serial.print("Distance: ");
//  Serial.println(dist);
//  clearRxBuffer();
  readURMDist(l,numSensors,distances);
  delay(500);
}
 

