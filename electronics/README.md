##BeoBot3
####Electronics
---

####Architecture:

####Resources:

* Schematic:
* Datasheets:


####Known Issues:
* Remove RS485 RX/TX jumpers while programming Nano-2. This connection is shared between programming interface (Serial) and MAX1348 (RS485 transciver).
* XBEE should always be on TTL1 port.
* Ignore TTL3/IMU markings on GMR board. 10-DOF IMU connects as I2C slave to Mega.
* ROS/Arduino Mega not responding:
	- Solution: This happens because Mega can not talk to nano-1 or nano-2. In the setup phase, Mega looks for I2C slaves to be connected. 
	- Also, make sure GMR board is powered ON. I2C pull up resistors use VCC from GMR board, not from USB connections.
* GMR Board has SX3478 and schematic has GS3478. This are pin compatible alternatives.