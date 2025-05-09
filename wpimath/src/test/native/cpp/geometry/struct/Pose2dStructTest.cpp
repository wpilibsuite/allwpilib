// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "frc/geometry/Pose2d.h"

using namespace frc;

namespace {

using StructType = wpi::Struct<frc::Pose2d>;
const Pose2d EXPECTED_DATA{
    Pose2d{Translation2d{0.191_m, 2.2_m}, Rotation2d{22.9_rad}}};
}  // namespace

TEST(Pose2dStructTest, Roundtrip) {
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  StructType::Pack(buffer, EXPECTED_DATA);

  Pose2d unpacked_data = StructType::Unpack(buffer);

  EXPECT_EQ(EXPECTED_DATA.Translation(), unpacked_data.Translation());
  EXPECT_EQ(EXPECTED_DATA.Rotation(), unpacked_data.Rotation());
}
