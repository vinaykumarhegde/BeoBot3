/***************************************************************
   Motor driver function definitions - by James Nugen
   *************************************************************/

#ifndef motor_driver_h
#define motor_driver_h

void initMotorController();
void setMotorSpeed(int i, int spd);
void setMotorSpeeds(int leftSpeed, int rightSpeed);

#ifdef DFRHCR_N1_ENCODER
  // Nano 1 Motor control attributes.
  #ifndef N1_I2C_ADDR
    #define N1_I2C_ADDR 0x10
  #endif
  #ifndef N2_I2C_ADDR
    #define N2_I2C_ADDR 0x20
  #endif
#endif

#endif
