// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "frc/geometry/Translation2d.h"

using namespace frc;

namespace {

using StructType = wpi::Struct<frc::Translation2d>;
const Translation2d EXPECTED_DATA{Translation2d{3.504_m, 22.9_m}};
}  // namespace

TEST(Translation2dStructTest, Roundtrip) {
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  StructType::Pack(buffer, EXPECTED_DATA);

  Translation2d unpacked_data = StructType::Unpack(buffer);

  EXPECT_EQ(EXPECTED_DATA.X(), unpacked_data.X());
  EXPECT_EQ(EXPECTED_DATA.Y(), unpacked_data.Y());
}
