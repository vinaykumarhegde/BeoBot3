#include "Arduino.h"
#define printByte(args) Serial.write(args)

#define ADDSETCMDLEN 7 // Address Set Command Length
#define ADDSETHSLEN 7 // Address Set Handshake Response Length
#define TRIGGERDELAY 30 // milliseconds to wait after trigger
#define READDELAY 10 // milliseconds to wait after read signal

class URM04Sensor;
bool valAddResp(byte, byte);
void setAdd(byte);
void clearRxBuffer();
void initURM(URM04Sensor[],uint8_t,byte);

enum sensors {
 UR_L2N,
 UR_L2NW,
 UR_L2NE,
 UR_L2S,
 UR_L2SW,
 UR_L2SE
};



void clearRxBuffer(){
  while(Serial.available())
    Serial.read();
}

bool valAddResp(byte received[], byte address){
  byte cmds[ADDSETHSLEN] = {0x55,0xaa,address,0x01,0x55,0x01,0x00};
  for(int i=0;i<ADDSETHSLEN;i++){
    if(cmds[i] != received[i])
      return false;
    cmds[ADDSETHSLEN-1] += received[i];
  }
  return true;
}

void setAdd(byte address){
  byte cmds[ADDSETCMDLEN] = {0x55,0xaa,0xab,0x01,0x55,address,0x00};
  byte received[ADDSETHSLEN];
  Serial.flush();
  for(int i=0; i < ADDSETCMDLEN; i++){
    Serial.write(cmds[i]);
    cmds[ADDSETCMDLEN-1] += cmds[i];
  }
  delay(2);
  uint8_t tries = 0;
  while(tries < 5){
    if(Serial.available()==ADDSETHSLEN){ // Serial handshake received.
      for(int i=0;i<ADDSETHSLEN;++i) received[i] = Serial.read();
      if(valAddResp(received, address)){
        Serial.println("Address setting Successful.");
        break;
      }
      delay(100);
    } else {
      // Serial return not found.
      delay(100);
    }
    Serial.println("Retrying to set address...");
    tries++;
  }
}



/****************** URM04 Sensor Class ****************/
class URM04Sensor{
  private:
    byte _add; // Address of sensor
    HardwareSerial* _ser = &Serial;
    byte cmd[6] = {0x55,0xaa,0x00,0x00,0x00,0x00};
    byte *received;
    uint8_t cmdLen = 6; uint8_t respLen = 8;
    void _transmit(){// Transmits the contents of cmd[] array
      for(int i=0;i<cmdLen;++i){
        printByte(cmd[i]);
        cmd[cmdLen-1] += cmd[i];
      }
      for(int i=4;i<cmdLen;i++) cmd[i]=0;
    }
    bool _verify(){
      byte cSum = 0;
      for(int i=0; i < respLen -1 ; ++i ) cSum += received[i];
      if(received[0] == 0x55 && received[1] == 0xaa)
        if(received[2] == _add && received[3] == 0x02 && received[4] == 0x02)
          if(received[7] == cSum)
            return true;
      return false;
    }
  public:
    uint8_t maxTries = 5;
    URM04Sensor(){
      _add = 0x11;cmd[2] = 0x11; // Set to default address
    }
    URM04Sensor(byte add){
      _add = add;cmd[2] = add;
    }
    void setAdd(byte add){
      _add = add;cmd[2] = add;
    }
    void Init(){
      _ser->print("Initialised URM04 sensor with address: ");
      _ser->println(_add,HEX);
    }
    void Trigger(){ // Triggers URM04 with address: _add;
      cmd[4] = 0x01;
      _transmit();
#ifdef DEBUG
      _ser->println("Issued Trigger Signal to URM04");
#endif      
    }
    void ReadSignal(){ // Issue Read Signal to URM
      cmd[4] = 0x02;
      _transmit();
    }
    uint16_t ReadDistance(){ // ReadSignal should be issued before this.
      uint8_t tryNo = 0;
      while(tryNo < maxTries){
        if(_ser->available() == respLen){ // Enough bytes are present in the rx buffer.
          received = new byte[respLen]; // Create a memory to store received bytes.
          for(int i=0;i<respLen;++i) received[i] = _ser->read(); // Read the serial bytes
          _ser->flush();
          if(_verify()){ // Checksum and other headers are correct
            uint16_t dist = received[5]*256 + received[6];
#ifdef DEBUG
            _ser->print("Distance sensed by URM04 at ");
            _ser->print(_add); 
            _ser->print(" is: ");
            _ser->println(dist);
#endif            
            return dist;
          } else {
#ifdef DEBUG
            _ser->println("Error in distance measurement. Returning 0xFFFF...");
#endif            
            return 0xFFFF;
          }
        } else {
          // Not enough bytes in the buffer. Continue...
        }
        tryNo++;
      }
      return 0xFFFF;
    }
};

void initURM(URM04Sensor l[], uint8_t num, byte startAdd){
  for(int i=0; i<num; i++){
    l[i].setAdd(startAdd + i);
    l[i].Init();
  }
}

