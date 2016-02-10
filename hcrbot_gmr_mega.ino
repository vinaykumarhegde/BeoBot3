#include <Wire.h>

#define N1_I2C_ADDR 0x10
#define N2_I2C_ADDR 0x20
#define CMDBUFFER 10 // Command buffer.
unsigned int cmdlen = 0;
char i2c_cmdline[CMDBUFFER];

void processCmd(){
  Serial.println("Found command");
  Serial.print("Entered: "); Serial.print(cmdlen);
  Serial.print('\n');
  switch(i2c_cmdline[0]){
    case 'A':
    case 'a': 
        {Serial.println("Command with A");
         Wire.beginTransmission(N1_I2C_ADDR);
         Wire.write("A123");
         Wire.endTransmission(N1_I2C_ADDR);
         Wire.requestFrom(N1_I2C_ADDR,1);
         int bytes = Wire.available();
         Serial.print("Slave sent ");
         Serial.print(bytes);
         Serial.print(" of information\n");
         for(int i = 0; i< bytes; i++)
         {
            char x = Wire.read();
            Serial.print("Slave Sent: ");
            Serial.print(x, HEX);
            Serial.print("\n");
         }
        }  
         break;
    case 'B':
    case 'b': 
         {
          Serial.println("Command with B");
         Wire.beginTransmission(N1_I2C_ADDR);
         Wire.write("B123");
         Wire.endTransmission(N1_I2C_ADDR);
         Wire.requestFrom(N1_I2C_ADDR,1);
         delay(500);
         int bytes = Wire.available();
         Serial.print("Slave sent ");
         Serial.print(bytes);
         Serial.print(" of information\n");
         for(int i = 0; i< bytes; i++)
         {
            char x = Wire.read();
            Serial.print("Slave Sent: ");
            Serial.print(x, HEX);
            Serial.print("\n");
         } 
         }
         break;
    break;
    case 'C':
    case 'c':
         { Serial.println("Command with C");
         Wire.beginTransmission(N1_I2C_ADDR);
         Wire.write("C123");
         Wire.endTransmission(N1_I2C_ADDR);
         Wire.requestFrom(N1_I2C_ADDR,1);
         int bytes = Wire.available();
         Serial.print("Slave sent ");
         Serial.print(bytes);
         Serial.print(" of information\n");
         for(int i = 0; i< bytes; i++)
         {
            char x = Wire.read();
            Serial.print("Slave Sent: ");
            Serial.print(x, HEX);
            Serial.print("\n");
         } 
         } 
         break;
    default: Serial.println("Something else.");
  }
  cmdlen = 0;
  memset(i2c_cmdline,0,CMDBUFFER);
}


void setup() {
  // put your setup code here, to run once:
  Wire.begin(); 
  Serial.begin(115200);
  Serial.println("Master rogram Started...");
}

void loop() {
  // put your main code here, to run repeatedly:
  while(Serial.available() && cmdlen < CMDBUFFER){
    char c = Serial.read();
    if(c != '\n'){
      i2c_cmdline[cmdlen++] = c;
    } else {
      processCmd();
    }
  }
  
}
