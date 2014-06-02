/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WPILib.h"
#include "gtest/gtest.h"

/**
 * A class to represent the camera with tilt and pan servos and a gyro
 * 
 * @author Thomas Clark
 */
class TiltPanCameraTest : public testing::Test {
protected:	
	Servo *m_tilt, *m_pan;
	Gyro *m_gyro;
	
	static const double RESET_TIME = 1.0;
	
	virtual void SetUp() {
		m_gyro = new Gyro(1);
		m_gyro->SetSensitivity(0.007);
		
		Wait(RESET_TIME);
		
		m_tilt = new Servo(10);
		m_pan = new Servo(9);
	}
	
	virtual void TearDown() {
		delete m_tilt;
		delete m_pan;
		delete m_gyro;
	}
	
	void Reset() {
		m_tilt->SetAngle(0.0);
		m_pan->SetAngle(0.0);
		
		Wait(RESET_TIME);
		
		m_gyro->Reset();
	}
};


/**
 * Test if the servo turns 180 degrees and the gyroscope measures this angle
 */
TEST_F(TiltPanCameraTest, GyroAngle) {
	const double TEST_ANGLE = 180.0;
	
	Reset();
	
	for(int i = 0; i < 100; i++) {
		m_pan->Set(i / 100.0);
		Wait(0.05);
	}
	
	double gyroAngle = m_gyro->GetAngle();
	
	double error = std::abs(TEST_ANGLE - gyroAngle);
	
	ASSERT_LE(error, 10.0) << "Gyro measured " << gyroAngle
		<< " degrees, servo should have turned " << TEST_ANGLE <<  " degrees"; 
}


