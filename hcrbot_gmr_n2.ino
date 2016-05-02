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
 
#include "URM04Driver.h"
#include <Wire.h>
#define N2_I2C_ADDR 0x11
#define BAUDRATE 115200

#define DEBUG 1


const uint8_t numSensors = 6; // Number of sensors
const byte initAdd = 0x11; // Initial address of sensor bunch.
URM04Sensor l[numSensors]; // URM04 sensors.
byte distances[2*numSensors]; // 2 bytes for each sensor distance data.

/* Send the URM readings through I2C */
void sendURMReadings(){
#ifdef DEBUG
  Serial.println("Sending URM Readings...");
#endif
  Wire.write(distances,2*numSensors);
}

void setup(){
  Serial.begin(BAUDRATE);
  Serial.println("Initalizing....");
  initURM(l,numSensors,initAdd);
  Wire.begin(N2_I2C_ADDR);
  Wire.onRequest(sendURMReadings);
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
#ifdef DEBUG
    Serial.print("Distance (sensor:");
    Serial.print(i); Serial.print(": ");
    Serial.println(di);
#endif
    clearRxBuffer();
  }
}

void loop(){
  readURMDist(l,numSensors,distances);
  delay(500);
}
 

