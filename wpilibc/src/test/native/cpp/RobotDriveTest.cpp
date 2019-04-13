/*----------------------------------------------------------------------------*/
/* Copyright (c) 2014-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "MockSpeedController.h"
#include "frc/RobotDrive.h"
#include "frc/drive/DifferentialDrive.h"
#include "frc/drive/MecanumDrive.h"
#include "gtest/gtest.h"

using namespace frc;

class RobotDriveTest : public testing::Test {
 protected:
  MockSpeedController m_rdFrontLeft;
  MockSpeedController m_rdRearLeft;
  MockSpeedController m_rdFrontRight;
  MockSpeedController m_rdRearRight;
  MockSpeedController m_frontLeft;
  MockSpeedController m_rearLeft;
  MockSpeedController m_frontRight;
  MockSpeedController m_rearRight;
  frc::RobotDrive m_robotDrive{m_rdFrontLeft, m_rdRearLeft, m_rdFrontRight,
                               m_rdRearRight};
  frc::DifferentialDrive m_differentialDrive{m_frontLeft, m_frontRight};
  frc::MecanumDrive m_mecanumDrive{m_frontLeft, m_rearLeft, m_frontRight,
                                   m_rearRight};

  double m_testJoystickValues[9] = {-1.0, -0.9, -0.5, -0.01, 0.0,
                                    0.01, 0.5,  0.9,  1.0};
  double m_testGyroValues[19] = {0,    45,   90,   135,  180, 225,  270,
                                 305,  360,  540,  -45,  -90, -135, -180,
                                 -225, -270, -305, -360, -540};
};

TEST_F(RobotDriveTest, TankDrive) {
  int joystickSize = sizeof(m_testJoystickValues) / sizeof(double);
  double leftJoystick, rightJoystick;
  m_differentialDrive.SetDeadband(0.0);
  m_differentialDrive.SetSafetyEnabled(false);
  m_mecanumDrive.SetSafetyEnabled(false);
  m_robotDrive.SetSafetyEnabled(false);
  for (int i = 0; i < joystickSize; i++) {
    for (int j = 0; j < joystickSize; j++) {
      leftJoystick = m_testJoystickValues[i];
      rightJoystick = m_testJoystickValues[j];
      m_robotDrive.TankDrive(leftJoystick, rightJoystick, false);
      m_differentialDrive.TankDrive(leftJoystick, rightJoystick, false);
      ASSERT_NEAR(m_rdFrontLeft.Get(), m_frontLeft.Get(), 0.01);
      ASSERT_NEAR(m_rdFrontRight.Get(), m_frontRight.Get(), 0.01);
    }
  }
}

TEST_F(RobotDriveTest, TankDriveSquared) {
  int joystickSize = sizeof(m_testJoystickValues) / sizeof(double);
  double leftJoystick, rightJoystick;
  m_differentialDrive.SetDeadband(0.0);
  m_differentialDrive.SetSafetyEnabled(false);
  m_mecanumDrive.SetSafetyEnabled(false);
  m_robotDrive.SetSafetyEnabled(false);
  for (int i = 0; i < joystickSize; i++) {
    for (int j = 0; j < joystickSize; j++) {
      leftJoystick = m_testJoystickValues[i];
      rightJoystick = m_testJoystickValues[j];
      m_robotDrive.TankDrive(leftJoystick, rightJoystick, true);
      m_differentialDrive.TankDrive(leftJoystick, rightJoystick, true);
      ASSERT_NEAR(m_rdFrontLeft.Get(), m_frontLeft.Get(), 0.01);
      ASSERT_NEAR(m_rdFrontRight.Get(), m_frontRight.Get(), 0.01);
    }
  }
}

TEST_F(RobotDriveTest, ArcadeDriveSquared) {
  int joystickSize = sizeof(m_testJoystickValues) / sizeof(double);
  double moveJoystick, rotateJoystick;
  m_differentialDrive.SetDeadband(0.0);
  m_differentialDrive.SetSafetyEnabled(false);
  m_mecanumDrive.SetSafetyEnabled(false);
  m_robotDrive.SetSafetyEnabled(false);
  for (int i = 0; i < joystickSize; i++) {
    for (int j = 0; j < joystickSize; j++) {
      moveJoystick = m_testJoystickValues[i];
      rotateJoystick = m_testJoystickValues[j];
      m_robotDrive.ArcadeDrive(moveJoystick, rotateJoystick, true);
      m_differentialDrive.ArcadeDrive(moveJoystick, -rotateJoystick, true);
      ASSERT_NEAR(m_rdFrontLeft.Get(), m_frontLeft.Get(), 0.01);
      ASSERT_NEAR(m_rdFrontRight.Get(), m_frontRight.Get(), 0.01);
    }
  }
}

TEST_F(RobotDriveTest, ArcadeDrive) {
  int joystickSize = sizeof(m_testJoystickValues) / sizeof(double);
  double moveJoystick, rotateJoystick;
  m_differentialDrive.SetDeadband(0.0);
  m_differentialDrive.SetSafetyEnabled(false);
  m_mecanumDrive.SetSafetyEnabled(false);
  m_robotDrive.SetSafetyEnabled(false);
  for (int i = 0; i < joystickSize; i++) {
    for (int j = 0; j < joystickSize; j++) {
      moveJoystick = m_testJoystickValues[i];
      rotateJoystick = m_testJoystickValues[j];
      m_robotDrive.ArcadeDrive(moveJoystick, rotateJoystick, false);
      m_differentialDrive.ArcadeDrive(moveJoystick, -rotateJoystick, false);
      ASSERT_NEAR(m_rdFrontLeft.Get(), m_frontLeft.Get(), 0.01);
      ASSERT_NEAR(m_rdFrontRight.Get(), m_frontRight.Get(), 0.01);
    }
  }
}

TEST_F(RobotDriveTest, MecanumCartesian) {
  int joystickSize = sizeof(m_testJoystickValues) / sizeof(double);
  int gyroSize = sizeof(m_testGyroValues) / sizeof(double);
  double xJoystick, yJoystick, rotateJoystick, gyroValue;
  m_mecanumDrive.SetDeadband(0.0);
  m_mecanumDrive.SetSafetyEnabled(false);
  m_differentialDrive.SetSafetyEnabled(false);
  m_robotDrive.SetSafetyEnabled(false);
  for (int i = 0; i < joystickSize; i++) {
    for (int j = 0; j < joystickSize; j++) {
      for (int k = 0; k < joystickSize; k++) {
        for (int l = 0; l < gyroSize; l++) {
          xJoystick = m_testJoystickValues[i];
          yJoystick = m_testJoystickValues[j];
          rotateJoystick = m_testJoystickValues[k];
          gyroValue = m_testGyroValues[l];
          m_robotDrive.MecanumDrive_Cartesian(xJoystick, yJoystick,
                                              rotateJoystick, gyroValue);
          m_mecanumDrive.DriveCartesian(xJoystick, -yJoystick, rotateJoystick,
                                        -gyroValue);
          ASSERT_NEAR(m_rdFrontLeft.Get(), m_frontLeft.Get(), 0.01)
              << "X: " << xJoystick << " Y: " << yJoystick
              << " Rotate: " << rotateJoystick << " Gyro: " << gyroValue;
          ASSERT_NEAR(m_rdFrontRight.Get(), -m_frontRight.Get(), 0.01)
              << "X: " << xJoystick << " Y: " << yJoystick
              << " Rotate: " << rotateJoystick << " Gyro: " << gyroValue;
          ASSERT_NEAR(m_rdRearLeft.Get(), m_rearLeft.Get(), 0.01)
              << "X: " << xJoystick << " Y: " << yJoystick
              << " Rotate: " << rotateJoystick << " Gyro: " << gyroValue;
          ASSERT_NEAR(m_rdRearRight.Get(), -m_rearRight.Get(), 0.01)
              << "X: " << xJoystick << " Y: " << yJoystick
              << " Rotate: " << rotateJoystick << " Gyro: " << gyroValue;
        }
      }
    }
  }
}

TEST_F(RobotDriveTest, MecanumPolar) {
  int joystickSize = sizeof(m_testJoystickValues) / sizeof(double);
  int gyroSize = sizeof(m_testGyroValues) / sizeof(double);
  double magnitudeJoystick, directionJoystick, rotateJoystick;
  m_mecanumDrive.SetDeadband(0.0);
  m_mecanumDrive.SetSafetyEnabled(false);
  m_differentialDrive.SetSafetyEnabled(false);
  m_robotDrive.SetSafetyEnabled(false);
  for (int i = 0; i < joystickSize; i++) {
    for (int j = 0; j < gyroSize; j++) {
      for (int k = 0; k < joystickSize; k++) {
        magnitudeJoystick = m_testJoystickValues[i];
        directionJoystick = m_testGyroValues[j];
        rotateJoystick = m_testJoystickValues[k];
        m_robotDrive.MecanumDrive_Polar(magnitudeJoystick, directionJoystick,
                                        rotateJoystick);
        m_mecanumDrive.DrivePolar(magnitudeJoystick, directionJoystick,
                                  rotateJoystick);
        ASSERT_NEAR(m_rdFrontLeft.Get(), m_frontLeft.Get(), 0.01)
            << "Magnitude: " << magnitudeJoystick
            << " Direction: " << directionJoystick
            << " Rotate: " << rotateJoystick;
        ASSERT_NEAR(m_rdFrontRight.Get(), -m_frontRight.Get(), 0.01)
            << "Magnitude: " << magnitudeJoystick
            << " Direction: " << directionJoystick
            << " Rotate: " << rotateJoystick;
        ASSERT_NEAR(m_rdRearLeft.Get(), m_rearLeft.Get(), 0.01)
            << "Magnitude: " << magnitudeJoystick
            << " Direction: " << directionJoystick
            << " Rotate: " << rotateJoystick;
        ASSERT_NEAR(m_rdRearRight.Get(), -m_rearRight.Get(), 0.01)
            << "Magnitude: " << magnitudeJoystick
            << " Direction: " << directionJoystick
            << " Rotate: " << rotateJoystick;
      }
    }
  }
}
