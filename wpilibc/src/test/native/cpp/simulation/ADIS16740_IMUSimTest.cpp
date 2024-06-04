// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>
#include <hal/HALBase.h>
#include <units/angular_velocity.h>

#include "frc/ADIS16470_IMU.h"
#include "frc/geometry/Rotation3d.h"
#include "frc/simulation/ADIS16470_IMUSim.h"

namespace frc::sim {
using IMUAxis = ADIS16470_IMU::IMUAxis;
TEST(ADIS16470_IMUSimTest, SetAttributes) {
  HAL_Initialize(500, 0);

  ADIS16470_IMU gyro{};
  ADIS16470_IMUSim sim{gyro};

  EXPECT_EQ(0_deg, gyro.GetAngle());
  EXPECT_EQ(0_deg, gyro.GetAngle(IMUAxis::kRoll));   // X
  EXPECT_EQ(0_deg, gyro.GetAngle(IMUAxis::kPitch));  // Y
  EXPECT_EQ(0_deg, gyro.GetAngle(IMUAxis::kYaw));    // Z
  EXPECT_EQ(0_deg_per_s, gyro.GetRate());

  constexpr units::degree_t TEST_ANGLE{123.456};
  constexpr units::degrees_per_second_t TEST_RATE{229.3504};
  sim.SetGyroAngleZ(TEST_ANGLE);
  sim.SetGyroRateZ(TEST_RATE);
  EXPECT_EQ(TEST_ANGLE, gyro.GetAngle());
  EXPECT_EQ(TEST_RATE, gyro.GetRate());
}

TEST(ADIS16470_IMUSimTest, Offset) {
  HAL_Initialize(500, 0);

  ADIS16470_IMU gyro{};
  ADIS16470_IMUSim sim{gyro};

  frc::Rotation3d OFFSET_Z{0_deg, 0_deg, 90_deg};
  gyro.Reset(OFFSET_Z);
  EXPECT_EQ(OFFSET_Z, gyro.GetRotation3d());  // default is Z axis (yaw)

  frc::Rotation3d OFFSET_YZ{0_deg, 90_deg, 90_deg};
  gyro.Reset(OFFSET_YZ);
  EXPECT_EQ(OFFSET_YZ, gyro.GetRotation3d());
  EXPECT_EQ(90_deg, gyro.GetAngle(IMUAxis::kPitch));
}
}  // namespace frc::sim
