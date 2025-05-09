// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "frc/kinematics/SwerveModulePosition.h"

using namespace frc;

namespace {

using StructType = wpi::Struct<frc::SwerveModulePosition>;
const SwerveModulePosition EXPECTED_DATA{
    SwerveModulePosition{3.504_m, Rotation2d{17.4_rad}}};
}  // namespace

TEST(SwerveModulePositionStructTest, Roundtrip) {
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  StructType::Pack(buffer, EXPECTED_DATA);

  SwerveModulePosition unpacked_data = StructType::Unpack(buffer);

  EXPECT_EQ(EXPECTED_DATA.distance.value(), unpacked_data.distance.value());
  EXPECT_EQ(EXPECTED_DATA.angle, unpacked_data.angle);
}
