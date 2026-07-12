// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "wpi/math/geometry/Rotation2d.hpp"

using namespace wpi::math;

namespace {

using StructType = wpi::util::Struct<wpi::math::Rotation2d>;
const Rotation2d kExpectedData{Rotation2d{1.91_rad}};
}  // namespace

TEST(Rotation2dStructTest, Roundtrip) {
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  StructType::Pack(buffer, kExpectedData);

  Rotation2d unpacked_data = StructType::Unpack(buffer);

  EXPECT_EQ(kExpectedData.Radians(), unpacked_data.Radians());
}
