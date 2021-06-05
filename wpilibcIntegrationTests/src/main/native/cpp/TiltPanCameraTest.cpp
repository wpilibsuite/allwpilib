// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cmath>

#include <units/time.h>

#include "TestBench.h"
#include "frc/ADXL345_SPI.h"
#include "frc/AnalogGyro.h"
#include "frc/Servo.h"
#include "frc/Timer.h"
#include "gtest/gtest.h"

static constexpr double kTiltSetpoint0 = 0.22;
static constexpr double kTiltSetpoint45 = 0.45;
static constexpr double kTiltSetpoint90 = 0.68;

/**
 * A fixture for the camera with two servos and a gyro
 */
class TiltPanCameraTest : public testing::Test {
 protected:
  frc::Servo m_tilt{TestBench::kCameraTiltChannel};
  frc::Servo m_pan{TestBench::kCameraPanChannel};
  frc::ADXL345_SPI m_spiAccel{frc::SPI::kOnboardCS1};

  TiltPanCameraTest() {
    m_tilt.Set(kTiltSetpoint45);
    m_pan.SetAngle(0.0);

    // Wait for servos to reset
    frc::Wait(2_s);
  }

  /**
   * Test if the servo turns 90 degrees and the gyroscope measures this angle
   * Note servo on TestBench is not the same type of servo that servo class
   * was designed for so setAngle is significantly off. This has been calibrated
   * for the servo on the rig.
   */
  void GyroTests(frc::AnalogGyro& gyro) {
    gyro.SetSensitivity(0.013);

    gyro.Reset();

    // Accumulator needs a small amount of time to reset before being tested
    frc::Wait(1_ms);

    // Verify  the gyro angle defaults to 0 immediately after being reset
    EXPECT_NEAR(0.0, gyro.GetAngle(), 1.0);

    // Make sure that the gyro doesn't get jerked when the servo goes to zero.
    m_pan.SetAngle(0.0);
    frc::Wait(0.5_s);
    gyro.Reset();

    for (int32_t i = 0; i < 600; i++) {
      m_pan.Set(i / 1000.0);
      frc::Wait(1_ms);
    }

    double gyroAngle = gyro.GetAngle();

    EXPECT_NEAR(gyroAngle, 90.0, 10.0)
        << "Gyro measured " << gyroAngle
        << " degrees, servo should have turned 90 degrees";
  }
};

TEST_F(TiltPanCameraTest, Gyro) {
  int cCenter;
  double cOffset;

  {
    frc::AnalogGyro gyro{TestBench::kCameraGyroChannel};
    GyroTests(gyro);

    // Gets calibrated parameters from previously calibrated gyro, allocates a
    // new gyro with the given parameters for center and offset, and re-runs
    // tests on the new gyro.
    cCenter = gyro.GetCenter();
    cOffset = gyro.GetOffset();
  }

  frc::AnalogGyro gyro{TestBench::kCameraGyroChannel, cCenter, cOffset};
  GyroTests(gyro);
}

/**
 * Test if the accelerometer measures gravity along the correct axes when the
 * camera rotates
 */
TEST_F(TiltPanCameraTest, SPIAccelerometer) {
  static constexpr auto kTiltTime = 1_s;
  static constexpr double kAccelerometerTolerance = 0.2;

  m_tilt.Set(kTiltSetpoint0);
  frc::Wait(kTiltTime);
  EXPECT_NEAR(-1.0, m_spiAccel.GetX(), kAccelerometerTolerance);
  EXPECT_NEAR(0.0, m_spiAccel.GetY(), kAccelerometerTolerance);
  EXPECT_NEAR(0.0, m_spiAccel.GetZ(), kAccelerometerTolerance);

  m_tilt.Set(kTiltSetpoint45);
  frc::Wait(kTiltTime);
  EXPECT_NEAR(-std::sqrt(0.5), m_spiAccel.GetX(), kAccelerometerTolerance);
  EXPECT_NEAR(0.0, m_spiAccel.GetY(), kAccelerometerTolerance);
  EXPECT_NEAR(std::sqrt(0.5), m_spiAccel.GetZ(), kAccelerometerTolerance);

  m_tilt.Set(kTiltSetpoint90);
  frc::Wait(kTiltTime);
  EXPECT_NEAR(0.0, m_spiAccel.GetX(), kAccelerometerTolerance);
  EXPECT_NEAR(0.0, m_spiAccel.GetY(), kAccelerometerTolerance);
  EXPECT_NEAR(1.0, m_spiAccel.GetZ(), kAccelerometerTolerance);
}
