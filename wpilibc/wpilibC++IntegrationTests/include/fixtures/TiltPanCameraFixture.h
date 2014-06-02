/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "WPILib.h"
#include "ITestFixture.h"

/**
 * A class to represent the camera with tilt and pan servos and a gyro
 * 
 * @author Thomas Clark
 */
class TiltPanCameraFixture : public ITestFixture {
public:
	TiltPanCameraFixture(Servo *tilt, Servo *pan, Gyro *gyro);
	
	virtual bool SetUp();
	virtual bool Reset();
	virtual bool TearDown();
	
	Gyro *GetGyro();
	Servo *GetTilt();
	Servo *GetPan();

protected:
	static const double RESET_TIME = 1.0;
	
	Servo *m_tilt, *m_pan;
	Gyro *m_gyro;
};
