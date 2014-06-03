/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WPILib.h"
#include "gtest/gtest.h"
#include "fixtures/TiltPanCameraFixture.h"
#include "TestBench.h"

class TiltPanCameraTest : public testing::Test {
protected:	
	TiltPanCameraFixture *m_fixture;
	
	virtual void SetUp() {
		m_fixture = TestBench::GetTiltPanCamera();
		
		m_fixture->SetUp();
	}
	
	virtual void TearDown() {
		m_fixture->TearDown();
	}
};


/**
 * Test if the servo turns 180 degrees and the gyroscope measures this angle
 */
TEST_F(TiltPanCameraTest, GyroAngle) {
	const double TEST_ANGLE = 180.0;
	
	m_fixture->Reset();
	
	for(int i = 0; i < 100; i++) {
		m_fixture->GetPan()->Set(i / 100.0);
		Wait(0.05);
	}
	
	double gyroAngle = m_fixture->GetGyro()->GetAngle();
	
	double error = std::abs(TEST_ANGLE - gyroAngle);
	
	ASSERT_LE(error, 10.0) << "Gyro measured " << gyroAngle
		<< " degrees, servo should have turned " << TEST_ANGLE <<  " degrees"; 
}


