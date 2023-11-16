// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "frc/kinematics/SwerveModulePosition.h"

using namespace frc;

namespace {

using StructType = wpi::Struct<frc::SwerveModulePosition>;
const SwerveModulePosition kExpectedData{
    SwerveModulePosition{3.504, Rotation2d{17.4}}};
}  // namespace

TEST(SwerveModulePositionStructTest, Roundtrip) {
  uint8_t buffer[StructType::kSize];
  std::memset(buffer, 0, StructType::kSize);
  StructType::Pack(buffer, kExpectedData);

  SwerveModulePosition unpacked_data = StructType::Unpack(buffer);

  EXPECT_EQ(kExpectedData.distance.value(), unpacked_data.distance.value());
  EXPECT_EQ(kExpectedData.angle.value(), unpacked_data.angle.value());
}
