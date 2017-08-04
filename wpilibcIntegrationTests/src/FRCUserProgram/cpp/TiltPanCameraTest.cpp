/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <cmath>

#include "ADXL345_SPI.h"
#include "AnalogGyro.h"
#include "Servo.h"
#include "TestBench.h"
#include "Timer.h"
#include "gtest/gtest.h"

using namespace frc;

static constexpr double kServoResetTime = 2.0;

static constexpr double kTestAngle = 90.0;

static constexpr double kTiltSetpoint0 = 0.22;
static constexpr double kTiltSetpoint45 = 0.45;
static constexpr double kTiltSetpoint90 = 0.68;
static constexpr double kTiltTime = 1.0;
static constexpr double kAccelerometerTolerance = 0.2;
static constexpr double kSensitivity = 0.013;

/**
 * A fixture for the camera with two servos and a gyro
 */
class TiltPanCameraTest : public testing::Test {
 protected:
  static AnalogGyro* m_gyro;
  Servo *m_tilt, *m_pan;
  Accelerometer* m_spiAccel;

  static void SetUpTestCase() {
    // The gyro object blocks for 5 seconds in the constructor, so only
    // construct it once for the whole test case
    m_gyro = new AnalogGyro(TestBench::kCameraGyroChannel);
    m_gyro->SetSensitivity(kSensitivity);
  }

  static void TearDownTestCase() { delete m_gyro; }

  void SetUp() override {
    m_tilt = new Servo(TestBench::kCameraTiltChannel);
    m_pan = new Servo(TestBench::kCameraPanChannel);
    m_spiAccel = new ADXL345_SPI(SPI::kOnboardCS1);

    m_tilt->Set(kTiltSetpoint45);
    m_pan->SetAngle(0.0);

    Wait(kServoResetTime);

    m_gyro->Reset();
  }

  void DefaultGyroAngle();
  void GyroAngle();
  void GyroCalibratedParameters();

  void TearDown() override {
    delete m_tilt;
    delete m_pan;
    delete m_spiAccel;
  }
};

AnalogGyro* TiltPanCameraTest::m_gyro = nullptr;

/**
 * Test if the gyro angle defaults to 0 immediately after being reset.
 */
void TiltPanCameraTest::DefaultGyroAngle() {
  EXPECT_NEAR(0.0, m_gyro->GetAngle(), 1.0);
}

/**
 * Test if the servo turns 90 degrees and the gyroscope measures this angle
 * Note servo on TestBench is not the same type of servo that servo class
 * was designed for so setAngle is significantly off. This has been calibrated
 * for the servo on the rig.
 */
void TiltPanCameraTest::GyroAngle() {
  // Make sure that the gyro doesn't get jerked when the servo goes to zero.
  m_pan->SetAngle(0.0);
  Wait(0.5);
  m_gyro->Reset();

  for (int32_t i = 0; i < 600; i++) {
    m_pan->Set(i / 1000.0);
    Wait(0.001);
  }

  double gyroAngle = m_gyro->GetAngle();

  EXPECT_NEAR(gyroAngle, kTestAngle, 10.0)
      << "Gyro measured " << gyroAngle << " degrees, servo should have turned "
      << kTestAngle << " degrees";
}

/**
  * Gets calibrated parameters from previously calibrated gyro, allocates a new
  * gyro with the given parameters for center and offset, and re-runs tests on
  * the new gyro.
  */
void TiltPanCameraTest::GyroCalibratedParameters() {
  uint32_t cCenter = m_gyro->GetCenter();
  double cOffset = m_gyro->GetOffset();
  delete m_gyro;
  m_gyro = new AnalogGyro(TestBench::kCameraGyroChannel, cCenter, cOffset);
  m_gyro->SetSensitivity(kSensitivity);

  // Default gyro angle test
  // Accumulator needs a small amount of time to reset before being tested
  m_gyro->Reset();
  Wait(0.001);
  EXPECT_NEAR(0.0, m_gyro->GetAngle(), 1.0);

  // Gyro angle test
  // Make sure that the gyro doesn't get jerked when the servo goes to zero.
  m_pan->SetAngle(0.0);
  Wait(0.5);
  m_gyro->Reset();

  for (int32_t i = 0; i < 600; i++) {
    m_pan->Set(i / 1000.0);
    Wait(0.001);
  }

  double gyroAngle = m_gyro->GetAngle();

  EXPECT_NEAR(gyroAngle, kTestAngle, 10.0)
      << "Gyro measured " << gyroAngle << " degrees, servo should have turned "
      << kTestAngle << " degrees";
}

/**
 * Run all gyro tests in one function to make sure they are run in order.
 */
TEST_F(TiltPanCameraTest, TestAllGyroTests) {
  DefaultGyroAngle();
  GyroAngle();
  GyroCalibratedParameters();
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
