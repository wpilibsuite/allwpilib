// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "wpi/math/kinematics/SwerveModuleVelocity.hpp"

using namespace wpi::math;

namespace {

using StructType = wpi::util::Struct<wpi::math::SwerveModuleVelocity>;
const SwerveModuleVelocity kExpectedData{
    SwerveModuleVelocity{22.9_mps, Rotation2d{3.3_rad}}};
}  // namespace

TEST(SwerveModuleVelocityStructTest, Roundtrip) {
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  StructType::Pack(buffer, kExpectedData);

  SwerveModuleVelocity unpacked_data = StructType::Unpack(buffer);

  EXPECT_EQ(kExpectedData.velocity.value(), unpacked_data.velocity.value());
  EXPECT_EQ(kExpectedData.angle, unpacked_data.angle);
}
