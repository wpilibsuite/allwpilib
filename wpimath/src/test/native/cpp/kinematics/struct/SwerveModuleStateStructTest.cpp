// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "frc/kinematics/SwerveModuleState.h"

using namespace frc;

namespace {

using StructType = wpi::Struct<frc::SwerveModuleState>;
const SwerveModuleState EXPECTED_DATA{
    SwerveModuleState{22.9_mps, Rotation2d{3.3_rad}}};
}  // namespace

TEST(SwerveModuleStateStructTest, Roundtrip) {
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  StructType::Pack(buffer, EXPECTED_DATA);

  SwerveModuleState unpacked_data = StructType::Unpack(buffer);

  EXPECT_EQ(EXPECTED_DATA.speed.value(), unpacked_data.speed.value());
  EXPECT_EQ(EXPECTED_DATA.angle, unpacked_data.angle);
}
