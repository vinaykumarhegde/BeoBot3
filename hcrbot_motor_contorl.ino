/*
 * Program for Nano 1
 * 
 */

#include <Wire.h>
//#define LOGGING 1
#undef LOGGING


// Pins
#define R_PWM 9  //M1 Speed Control
#define L_PWM 11 //M2 Speed Control
#define R_DIR 8  //M1 Direction Control
#define L_DIR 10 //M1 Direction Control
#define R_ENC_OTHER 4 // Encoder Pin A
#define R_ENC_INT 2 // Int 0; Encode Pin B
#define L_ENC_INT 3 // Int 1; Encoder Pin A
#define L_ENC_OTHER 5 //Encode Pin B

#define MINPWM 50 //Minimum PWM for the motor, below this static friction stops the motor from spinning
#define STATICPULSEWIDTH 100 //Pulse time to avoid static friction in gears.

// Buffers
#define CMDBUFFER 10 // Command buffer.

// Other
#define MAXSPEED 100 // In mm/s
#define BASEWIDTH 285 // in mm
#define WHEELDIAMETER 136 // in mm
#define N1_I2C_ADDR 0x10
#define BAUDRATE 115200


// Variables
volatile long lTicks;
volatile long rTicks;

void leftTicks(){
  if(digitalRead(L_ENC_OTHER))
    lTicks--;
  else
    lTicks++;
}

// Clockwise rotation will increment ticks and 
// anti clockwise will decrease the count
void rightTicks(){
  if(digitalRead(R_ENC_OTHER))
    rTicks++;
  else
    rTicks--;
}

void readPWMInput(int bytes){
  byte inArray[4];
  if(Wire.available() == 4){
    for(int i=0;i<4;i++)
      inArray[i] = Wire.read();
  }
  else if(Wire.available() == 1){
    // Reset command sent:
    char c = Wire.read();
    if(c == 'r'){
      rTicks = 0;
      return;
    }
    if(c == 'l'){
      lTicks = 0;
      return;
    }
    return;
  }
  else{
#ifdef LOGGING
  Serial.println("Incorrect I2C bytes sent");
#endif
  }
  int rPWM = (inArray[0]) + (int(inArray[1]) << 8);
  int lPWM = (inArray[2]) + (int(inArray[3]) << 8);

  bool rDir = true;
  bool lDir = false;

#ifdef LOGGING
  Serial.print("R: ");
  rDir?Serial.print("F "):Serial.print("B ");
  Serial.print(rPWM,HEX);
  Serial.print("\tL: ");
  lDir?Serial.print("B "):Serial.print("F ");
  Serial.print(lPWM,HEX);
  Serial.println("");
#endif  
  if(rPWM < 0){
    rPWM = -rPWM;
    rDir = false;
  }
  if(lPWM < 0){
    lPWM = -lPWM;
    lDir = true;
  }

  (rDir)? digitalWrite(R_DIR,LOW):digitalWrite(R_DIR,HIGH);
  if(rPWM > 255)
    rPWM = 255;
  if(rPWM > 0 && rPWM < MINPWM){
    analogWrite(R_PWM, uint8_t(MINPWM));
    delay(STATICPULSEWIDTH);
  }
  analogWrite(R_PWM,uint8_t(rPWM));
  
  (lDir)? digitalWrite(L_DIR,LOW):digitalWrite(L_DIR,HIGH);
  if(lPWM > 255)
    lPWM = 255;
  if(lPWM > 0 && lPWM < MINPWM){
    analogWrite(L_PWM, uint8_t(MINPWM));
    delay(STATICPULSEWIDTH);
  }
  analogWrite(L_PWM,uint8_t(lPWM));  
}

void sendEncReadings(){
  uint8_t ticks[8];

  ticks[0] = (rTicks) & 0xFF;
  ticks[1] = (rTicks >> 8) & 0xFF;
  ticks[2] = (rTicks >> 16) & 0xFF;
  ticks[3] = (rTicks >> 24) & 0xFF;

  ticks[4] = (lTicks) & 0xFF;
  ticks[5] = (lTicks >> 8) & 0xFF;
  ticks[6] = (lTicks >> 16) & 0xFF;
  ticks[7] = (lTicks >> 24) & 0xFF;

#ifdef LOGGING
//  Serial.print("Sending Encoder value: ");
//  for(int i=3; i>=0;i--)
//    Serial.print(ticks[i],HEX);
//  Serial.print("\t");
//  for(int i=7; i>=4;i--)
//    Serial.print(ticks[i],HEX);
//  Serial.println(".");
//  Serial.print(rTicks);
//  Serial.print("\t");
//  Serial.println(lTicks);
#endif

  // Send 8 bytes of ticks data
  Wire.write(ticks,8);
}

void setup() {
  // put your setup code here, to run once:
#ifdef LOGGING
  Serial.begin(BAUDRATE);
  Serial.println("Serial monitor for Nano-1 started...");
#endif
  Wire.begin(N1_I2C_ADDR);
  Wire.onReceive(readPWMInput);
  Wire.onRequest(sendEncReadings);
  attachInterrupt(1, leftTicks, RISING);
  attachInterrupt(0, rightTicks, RISING);

  pinMode(R_PWM,OUTPUT);
  pinMode(L_PWM,OUTPUT);
  pinMode(R_DIR,OUTPUT);
  pinMode(L_DIR,OUTPUT);

  pinMode(R_ENC_OTHER,INPUT);
  pinMode(L_ENC_OTHER,INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
#ifdef LOGGING
//  Serial.print("Right encoder: ");
//  Serial.print(rTicks);
//  Serial.print("\tLeft encoder: ");
//  Serial.print(lTicks);
//  Serial.println(".");
#endif
  delay(1000);

}
