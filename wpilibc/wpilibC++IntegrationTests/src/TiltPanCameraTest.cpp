/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WPILib.h"
#include "gtest/gtest.h"
#include "TestBench.h"

static constexpr double kResetTime = 1.0;
static constexpr double kTestAngle = 180.0;
	
/**
 * A fixture for the camera with two servos and a gyro
 * @author Thomas Clark
 */
class TiltPanCameraTest : public testing::Test {
protected:
	Servo *m_tilt, *m_pan;
	Gyro *m_gyro;
	
	virtual void SetUp() {
		m_tilt = new Servo(TestBench::kCameraTiltChannel);
		m_pan = new Servo(TestBench::kCameraPanChannel);
		m_gyro = new Gyro(TestBench::kCameraGyroChannel);
	}
	
	virtual void TearDown() {
		delete m_tilt;
		delete m_pan;
		delete m_gyro;
	}
	
	void Reset() {
		m_tilt->Set(0.0);
		m_pan->Set(0.0);

		Wait(kResetTime);

		m_gyro->Reset();
	}
};


/**
 * Test if the servo turns 180 degrees and the gyroscope measures this angle
 */
TEST_F(TiltPanCameraTest, GyroAngle) {
	Reset();
	
	for(int i = 0; i < 100; i++) {
		m_pan->Set(i / 100.0);
		Wait(0.05);
	}
	
	double gyroAngle = m_gyro->GetAngle();
	
	EXPECT_NEAR(gyroAngle, kTestAngle, 20.0) << "Gyro measured " << gyroAngle
		<< " degrees, servo should have turned " << kTestAngle <<  " degrees"; 
}


