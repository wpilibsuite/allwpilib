// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "wpimath/kinematics/SwerveModuleState.h"

using namespace wpimath;

namespace {

using StructType = wpi::Struct<wpimath::SwerveModuleState>;
const SwerveModuleState kExpectedData{
    SwerveModuleState{22.9_mps, Rotation2d{3.3_rad}}};
}  // namespace

TEST(SwerveModuleStateStructTest, Roundtrip) {
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  StructType::Pack(buffer, kExpectedData);

  SwerveModuleState unpacked_data = StructType::Unpack(buffer);

  EXPECT_EQ(kExpectedData.speed.value(), unpacked_data.speed.value());
  EXPECT_EQ(kExpectedData.angle, unpacked_data.angle);
}
