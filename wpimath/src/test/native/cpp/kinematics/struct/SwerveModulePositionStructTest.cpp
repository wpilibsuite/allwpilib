// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "wpi/math/kinematics/SwerveModulePosition.h"

using namespace wpi::math;

namespace {

using StructType = wpi::Struct<wpi::math::SwerveModulePosition>;
const SwerveModulePosition kExpectedData{
    SwerveModulePosition{3.504_m, Rotation2d{17.4_rad}}};
}  // namespace

TEST(SwerveModulePositionStructTest, Roundtrip) {
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  StructType::Pack(buffer, kExpectedData);

  SwerveModulePosition unpacked_data = StructType::Unpack(buffer);

  EXPECT_EQ(kExpectedData.distance.value(), unpacked_data.distance.value());
  EXPECT_EQ(kExpectedData.angle, unpacked_data.angle);
}
