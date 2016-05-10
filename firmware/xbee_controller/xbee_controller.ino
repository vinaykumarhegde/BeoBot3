/*
 * Arduino sketch for Gamepad
 * Author: Vinay (me@vnay.in)
 * 
 */


// Pins
// Analog pins 
#define Xbee Serial1
#define ROBOT_WIDTH 0.285 // 285
enum AnalogPins {JOYLEFBUT, JOYRITBUT, JOYRITVER, JOYRITHOR, JOYLEFVER, JOYLEFHOR, LASTANALOG };

enum DigitalPins {MOTOR = 2, SELECT, START, LEFTUP, 
				LEFTLT, LEFTDN, LEFTRT, RIGHT1, RIGHT4, 
				RIGHT2, RIGHT3, RIGHTZ1, RIGHTZ2, LEFTZ1, LEFTZ2, LASTDIGITAL};

uint8_t state[LASTDIGITAL]; // HIGH or LOW 
int16_t analog[LASTANALOG]; // 0 to 1023
unsigned long timer=0;
uint16_t duration = 50; // 500ms delay
volatile bool MotorEnable = false; // To enable movements
volatile bool Disabled = true; // Default Robot Disabled.
unsigned long startPressed = 0;
int linScale = 10; float linScaleInc = 1;
int angScale = 10; float angScaleInc = 1;
int linCal = 512;
int angCal = 512;


// Test

void InitializePins(){
	// Set Motor pin as output
	pinMode(MOTOR, OUTPUT); digitalWrite(MOTOR, LOW);
	for(int pin = SELECT; pin != LASTDIGITAL; ++pin){
		pinMode(pin, INPUT);
	}
}

void SpinMotor(unsigned long time){
	unsigned long now = millis();
	while(millis() - now < time){
		digitalWrite(MOTOR,HIGH);
	}
	digitalWrite(MOTOR,LOW);
}

void EnableRobot(){
	if(! state[START] ){
		if(millis()-startPressed > 1000) {
			Disabled = false;
			Serial.println("Robot Enabled.");
			SpinMotor(300);
		}
		return;
	}
	else{
		startPressed = millis();
	}
}

void EnableROS(){
  if(!state[SELECT]){
  	delay(200);
  	if(!state[SELECT]){
  		delay(200);
  		if(state[200]){
  			SpinMotor(300);
  			Xbee.write('!');
  			Xbee.write('!');
  			SpinMotor(300);
  		}
  	}
  }
}

void DisableRobot(){
	Disabled = true;
	Serial.println("Robot Disabled");
	SpinMotor(500);
}

void ReadDigital(){
	for(int pin=SELECT; pin != LASTDIGITAL; ++pin)
		state[pin] = digitalRead(pin);
}

void ReadAnalog(){
	for(int pin = JOYLEFBUT; pin != LASTANALOG; ++pin){
		analog[pin] = analogRead(pin);
	}
}

void CalibrateJostick(){
	Serial.println("Calibrating Jostick");
	linCal = analog[JOYLEFVER];
	angCal = analog[JOYRITHOR];

}

void SendCommands(){
	char temp[10];
	Serial.println("Wheels rolling...");
	float part1 = linScale*(analog[JOYLEFVER]-linCal)/1024;
	float part2 = angScale*(angCal - analog[JOYRITHOR])/1024;
	Serial.print(part1);Serial.print(","); Serial.print(part2);Serial.println(";");
	int lSpeed = int(part1 - part2);
	int rSpeed = int(part1 + part2);
	sprintf(temp, "m %d %d\r\n\0",rSpeed,lSpeed);
	for(int i=0; temp[i] != '\0'; i++){
		Xbee.print(temp[i]);
	}
}

void StopRobot(){
	char temp[10];
	Serial.println("Stopping Robot.");
	sprintf(temp, "m %d %d\r\0",0,0);
	for(int i=0; temp[i] != '\0'; i++){
		Xbee.print(temp[i]);
	}
}

void setup() {
	// Initialize Serial Ports:
	Serial.begin(115200); // USB<>SERIAL interface
	Xbee.begin(115200);  // XBEE interface
	// Initialize the pins:
	InitializePins();

// // Speed test:
// char temp[20];
// for(int i=0; i< 200; i++){
//  Serial.println(i);
//  sprintf(temp, "m %d %d\r\n\0",i,i);
//    for(int i=0; temp[i] != '\0'; i++){
//    Xbee.print(temp[i]);
//    }
//    delay(1000);
// }
}

void loop() {
	if(Disabled){
		ReadDigital();
		EnableRobot();
	}
	else if(millis()-timer > duration){
		ReadAnalog();
		ReadDigital();
		if(!state[RIGHT1]){
			StopRobot();
			DisableRobot();
		}
		else if(!state[SELECT]){
			EnableROS();
		}
		else if(!state[LEFTLT] && !state[RIGHTZ2]){
			angScale += angScaleInc;
			Serial.print("Angular velocity Scale: "); Serial.print(angScale);Serial.println(";");
		}
		else if(!state[LEFTRT] && !state[RIGHTZ2]){
			angScale -= angScaleInc;
			Serial.print("Angular velocity Scale: "); Serial.print(angScale);Serial.println(";");
		}
		else if(!state[LEFTUP] && !state[RIGHTZ2]){
			linScale += linScaleInc;
			Serial.print("Linear velocity Scale: "); Serial.print(linScale);Serial.println(";");
		}
		else if(!state[LEFTDN] && !state[RIGHTZ2]
			){
			linScale -= linScaleInc;
			Serial.print("Linear velocity Scale: "); Serial.print(linScale);Serial.println(";");
		}
		else if(!state[RIGHT2] && !state[LEFTZ2]){
			CalibrateJostick();
		}
		else if(!state[RIGHTZ1] ||  !state[LEFTZ1])
			StopRobot();
		else if(!state[RIGHTZ2] ||  !state[LEFTZ2]){
			SendCommands();
		}
		else {
			linScaleInc = 1;
			angScaleInc = 1;
		}
		timer = millis();
	}
}
