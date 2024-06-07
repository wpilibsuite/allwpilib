// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "frc/geometry/Ellipse2d.h"

using namespace frc;

namespace {

using StructType = wpi::Struct<frc::Ellipse2d>;
const Ellipse2d kExpectedData{
    Pose2d{Translation2d{0.191_m, 2.2_m}, Rotation2d{22.9_rad}}, 1.2_m, 2.3_m};
}  // namespace

TEST(Ellipse2dStructTest, Roundtrip) {
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  StructType::Pack(buffer, kExpectedData);

  Ellipse2d unpacked_data = StructType::Unpack(buffer);

  EXPECT_EQ(kExpectedData.Center(), unpacked_data.Center());
  EXPECT_EQ(kExpectedData.XSemiAxis(), unpacked_data.XSemiAxis());
  EXPECT_EQ(kExpectedData.YSemiAxis(), unpacked_data.YSemiAxis());
}
