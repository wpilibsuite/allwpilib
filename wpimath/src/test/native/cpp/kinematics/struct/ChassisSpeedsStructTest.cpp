// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "frc/kinematics/ChassisSpeeds.h"

using namespace frc;

namespace {

using StructType = wpi::Struct<frc::ChassisSpeeds>;
const ChassisSpeeds EXPECTED_DATA{
    ChassisSpeeds{2.29_mps, 2.2_mps, 0.3504_rad_per_s}};
}  // namespace

TEST(ChassisSpeedsStructTest, Roundtrip) {
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  StructType::Pack(buffer, EXPECTED_DATA);

  ChassisSpeeds unpacked_data = StructType::Unpack(buffer);

  EXPECT_EQ(EXPECTED_DATA.vx.value(), unpacked_data.vx.value());
  EXPECT_EQ(EXPECTED_DATA.vy.value(), unpacked_data.vy.value());
  EXPECT_EQ(EXPECTED_DATA.omega.value(), unpacked_data.omega.value());
}
