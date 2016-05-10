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
#include <Wire.h>

#define N2_I2C_ADDR 0x20
#define BAUDRATE 19200


const uint8_t numSensors = 6; // Number of sensors
const byte initAdd = 0x11; // Initial address of sensor bunch.
URM04Sensor l[numSensors]; // URM04 sensors.
byte distances[2*numSensors]; // 2 bytes for each sensor distance data.
bool urm_ready = false;
int i=0;
unsigned long times = 0;

void(* resetFunc) (void) = 0; //declare reset function @ address 0


void setup(){
  Serial.begin(BAUDRATE);
  Serial.println("Initalizing....");
//  setAdd(0x11);
  initURM(l,numSensors,initAdd);
// Init Wire
  Wire.begin(N2_I2C_ADDR);
  Wire.onRequest(sendURMReadings);
  times = millis();
}

void readURMDist(URM04Sensor l[], uint8_t num, byte dist[]){
  for(int i=0; i<num; ++i){
    l[i].Trigger();
    delay(TRIGGERDELAY);
    l[i].ReadSignal();
    delay(READDELAY);
    uint16_t di = l[i].ReadDistance();
    dist[2*i] = di/256; 
    dist[2*i+1] = di % 256; 
//    Serial.print("Reading: "); Serial.print(i); Serial.print(" dist: "); Serial.println(di);
    clearRxBuffer();
  }
  urm_ready = true;
}


void sendURMReadings(){
  if(!urm_ready)
    readURMDist(l,numSensors,distances);
  Serial.println("I2C request recieved. Sending distances...");
  printSerial(distances,numSensors);
  Wire.write(distances,2*numSensors);
  for(int i=0;i<100;i++)
    delay(1000);
}



void printSerial(byte dist[], uint8_t num){
  for(int i=0; i<num; ++i){
    uint16_t di = 256*dist[2*i]+dist[2*i+1];
    Serial.print("Distance sensor:");
    Serial.print(i); Serial.print(": ");
    Serial.println(di);
  }
}

void loop(){
  readURMDist(l,numSensors,distances);
  if(i++ == 10) {
    printSerial(distances,numSensors);
    Serial.print("Time in milliseconds: "); Serial.println(millis()-times);
    delay(50);
    resetFunc();
  }
  delay(50);
}
 
