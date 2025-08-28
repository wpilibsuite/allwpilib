// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "wpimath/geometry/Translation2d.h"

using namespace wpimath;

namespace {

using StructType = wpi::Struct<wpimath::Translation2d>;
const Translation2d kExpectedData{Translation2d{3.504_m, 22.9_m}};
}  // namespace

TEST(Translation2dStructTest, Roundtrip) {
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  StructType::Pack(buffer, kExpectedData);

  Translation2d unpacked_data = StructType::Unpack(buffer);

  EXPECT_EQ(kExpectedData.X(), unpacked_data.X());
  EXPECT_EQ(kExpectedData.Y(), unpacked_data.Y());
}
