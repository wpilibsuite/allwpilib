/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "TestBench.h"

TiltPanCameraFixture *TestBench::GetTiltPanCamera() {
	Gyro *gyro = new Gyro(1);
	gyro->SetSensitivity(0.007);
	
	Servo *tilt = new Servo(10),
		*pan = new Servo(9);
	
	return new TiltPanCameraFixture(tilt, pan, gyro);
}
