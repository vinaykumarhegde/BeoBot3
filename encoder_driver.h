/* *************************************************************
   Encoder driver function definitions - by James Nugen
   ************************************************************ */
   
#ifndef encoder_driver_h
#define encoder_driver_h

#ifdef ARDUINO_ENC_COUNTER
  //below can be changed, but should be PORTD pins; 
  //otherwise additional changes in the code are required
  #define LEFT_ENC_PIN_A PD2  //pin 2
  #define LEFT_ENC_PIN_B PD3  //pin 3
  
  //below can be changed, but should be PORTC pins
  #define RIGHT_ENC_PIN_A PC4  //pin A4
  #define RIGHT_ENC_PIN_B PC5   //pin A5
#endif

#ifdef DFRHCR_N1_ENCODER
  // Nano 1 Motor control attributes.
  #ifndef N1_I2C_ADDR
    #define N1_I2C_ADDR 0x10
  #endif
  #ifndef N2_I2C_ADDR
    #define N2_I2C_ADDR 0x20
  #endif
#endif
   
long readEncoder(int i);
void resetEncoder(int i);
void resetEncoders();

#endif

