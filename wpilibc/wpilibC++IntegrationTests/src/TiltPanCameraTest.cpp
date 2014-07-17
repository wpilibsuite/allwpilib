/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WPILib.h"
#include "gtest/gtest.h"
#include "TestBench.h"

static constexpr double kServoResetTime = 1.0;

static constexpr double kTestAngle = 180.0;

static constexpr double kTiltSetpoint0 = 0.16;
static constexpr double kTiltSetpoint45 = 0.385;
static constexpr double kTiltSetpoint90 = 0.61;
static constexpr double kTiltTime = 0.5;
static constexpr double kAccelerometerTolerance = 0.1;

/**
 * A fixture for the camera with two servos and a gyro
 * @author Thomas Clark
 */
class TiltPanCameraTest : public testing::Test {
protected:
	static Gyro *m_gyro;
	Servo *m_tilt, *m_pan;
	ADXL345_SPI *m_spiAccel;
	
	static void SetUpTestCase() {
		// The gyro object blocks for 5 seconds in the constructor, so only
		// construct it once for the whole test case
		m_gyro = new Gyro(TestBench::kCameraGyroChannel);
	}
	
	static void TearDownTestCase() {
		delete m_gyro;
	}
	
	virtual void SetUp() {
		m_tilt = new Servo(TestBench::kCameraTiltChannel);
		m_pan = new Servo(TestBench::kCameraPanChannel);
		m_spiAccel = new ADXL345_SPI(SPI::kOnboardCS0);
	}
	
	virtual void TearDown() {
		delete m_tilt;
		delete m_pan;
		delete m_spiAccel;
	}
};

Gyro *TiltPanCameraTest::m_gyro = 0;

/**
 * Test if the servo turns 180 degrees and the gyroscope measures this angle
 */
TEST_F(TiltPanCameraTest, GyroAngle) {
	Wait(kServoResetTime);
	
	for(int i = 0; i < 100; i++) {
		m_pan->Set(i / 100.0);
		Wait(0.05);
	}
	
	double gyroAngle = m_gyro->GetAngle();
	
	EXPECT_NEAR(gyroAngle, kTestAngle, 20.0) << "Gyro measured " << gyroAngle
		<< " degrees, servo should have turned " << kTestAngle <<  " degrees"; 
}

/**
 * Test if the accelerometer measures gravity along the correct axes when the
 * camera rotates
 */
TEST_F(TiltPanCameraTest, SPIAccelerometer) {
	m_tilt->Set(kTiltSetpoint0);
	Wait(kTiltTime);
	EXPECT_NEAR(-1.0, m_spiAccel->GetAcceleration(ADXL345_SPI::kAxis_X), kAccelerometerTolerance);
	EXPECT_NEAR(0.0, m_spiAccel->GetAcceleration(ADXL345_SPI::kAxis_Y), kAccelerometerTolerance);
	EXPECT_NEAR(0.0, m_spiAccel->GetAcceleration(ADXL345_SPI::kAxis_Z), kAccelerometerTolerance);
	
	m_tilt->Set(kTiltSetpoint45);
	Wait(kTiltTime);
	EXPECT_NEAR(-std::sqrt(0.5), m_spiAccel->GetAcceleration(ADXL345_SPI::kAxis_X), kAccelerometerTolerance);
	EXPECT_NEAR(0.0, m_spiAccel->GetAcceleration(ADXL345_SPI::kAxis_Y), kAccelerometerTolerance);
	EXPECT_NEAR(std::sqrt(0.5), m_spiAccel->GetAcceleration(ADXL345_SPI::kAxis_Z), kAccelerometerTolerance);
	
	m_tilt->Set(kTiltSetpoint90);
	Wait(kTiltTime);
	EXPECT_NEAR(0.0, m_spiAccel->GetAcceleration(ADXL345_SPI::kAxis_X), kAccelerometerTolerance);
	EXPECT_NEAR(0.0, m_spiAccel->GetAcceleration(ADXL345_SPI::kAxis_Y), kAccelerometerTolerance);
	EXPECT_NEAR(1.0, m_spiAccel->GetAcceleration(ADXL345_SPI::kAxis_Z), kAccelerometerTolerance);
}
