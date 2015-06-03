/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WPILib.h"
#include "gtest/gtest.h"
#include "TestBench.h"

static constexpr double kServoResetTime = 2.0;

static constexpr double kTestAngle = 90.0;

static constexpr double kTiltSetpoint0 = 0.22;
static constexpr double kTiltSetpoint45 = 0.45;
static constexpr double kTiltSetpoint90 = 0.68;
static constexpr double kTiltTime = 1.0;
static constexpr double kAccelerometerTolerance = 0.2;

/**
 * A fixture for the camera with two servos and a gyro
 * @author Thomas Clark
 */
class TiltPanCameraTest : public testing::Test {
protected:
	static Gyro *m_gyro;
	Servo *m_tilt, *m_pan;
	Accelerometer *m_spiAccel;

	static void SetUpTestCase() {
		// The gyro object blocks for 5 seconds in the constructor, so only
		// construct it once for the whole test case
		m_gyro = new Gyro(TestBench::kCameraGyroChannel);
		m_gyro->SetSensitivity(0.013);
	}

	static void TearDownTestCase() {
		delete m_gyro;
	}

	virtual void SetUp() {
		m_tilt = new Servo(TestBench::kCameraTiltChannel);
		m_pan = new Servo(TestBench::kCameraPanChannel);
		m_spiAccel = new ADXL345_SPI(SPI::kOnboardCS0);

		m_tilt->Set(kTiltSetpoint45);
		m_pan->SetAngle(0.0f);

		Wait(kServoResetTime);

		m_gyro->Reset();
	}

	virtual void TearDown() {
		delete m_tilt;
		delete m_pan;
		delete m_spiAccel;
	}
};

Gyro *TiltPanCameraTest::m_gyro = 0;

/**
 * Test if the gyro angle defaults to 0 immediately after being reset.
 */
TEST_F(TiltPanCameraTest, DefaultGyroAngle) {
	EXPECT_NEAR(0.0f, m_gyro->GetAngle(), 1.0f);
}

/**
 * Test if the servo turns 180 degrees and the gyroscope measures this angle
 */
TEST_F(TiltPanCameraTest, GyroAngle) {
  // Make sure that the gyro doesn't get jerked when the servo goes to zero.
  m_pan->SetAngle(0.0);
  Wait(0.25);
  m_gyro->Reset();

	for(int i = 0; i < 600; i++) {
		m_pan->Set(i / 1000.0);
		Wait(0.001);
	}

	double gyroAngle = m_gyro->GetAngle();

	EXPECT_NEAR(gyroAngle, kTestAngle, 10.0) << "Gyro measured " << gyroAngle
		<< " degrees, servo should have turned " << kTestAngle <<  " degrees";
}

/**
 * Test if the accelerometer measures gravity along the correct axes when the
 * camera rotates
 */
TEST_F(TiltPanCameraTest, SPIAccelerometer) {
	m_tilt->Set(kTiltSetpoint0);
	Wait(kTiltTime);
	EXPECT_NEAR(-1.0, m_spiAccel->GetX(), kAccelerometerTolerance);
	EXPECT_NEAR(0.0, m_spiAccel->GetY(), kAccelerometerTolerance);
	EXPECT_NEAR(0.0, m_spiAccel->GetZ(), kAccelerometerTolerance);

	m_tilt->Set(kTiltSetpoint45);
	Wait(kTiltTime);
	EXPECT_NEAR(-std::sqrt(0.5), m_spiAccel->GetX(), kAccelerometerTolerance);
	EXPECT_NEAR(0.0, m_spiAccel->GetY(), kAccelerometerTolerance);
	EXPECT_NEAR(std::sqrt(0.5), m_spiAccel->GetZ(), kAccelerometerTolerance);

	m_tilt->Set(kTiltSetpoint90);
	Wait(kTiltTime);
	EXPECT_NEAR(0.0, m_spiAccel->GetX(), kAccelerometerTolerance);
	EXPECT_NEAR(0.0, m_spiAccel->GetY(), kAccelerometerTolerance);
	EXPECT_NEAR(1.0, m_spiAccel->GetZ(), kAccelerometerTolerance);
}
