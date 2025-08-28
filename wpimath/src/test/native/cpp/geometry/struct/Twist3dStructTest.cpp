// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "wpi/math/geometry/Twist3d.h"

using namespace wpi::math;

namespace {

using StructType = wpi::Struct<wpi::math::Twist3d>;
const Twist3d kExpectedData{
    Twist3d{1.1_m, 2.29_m, 35.04_m, 0.174_rad, 19.1_rad, 4.4_rad}};
}  // namespace

TEST(Twist3dStructTest, Roundtrip) {
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  StructType::Pack(buffer, kExpectedData);

  Twist3d unpacked_data = StructType::Unpack(buffer);

  EXPECT_EQ(kExpectedData.dx.value(), unpacked_data.dx.value());
  EXPECT_EQ(kExpectedData.dy.value(), unpacked_data.dy.value());
  EXPECT_EQ(kExpectedData.dz.value(), unpacked_data.dz.value());
  EXPECT_EQ(kExpectedData.rx.value(), unpacked_data.rx.value());
  EXPECT_EQ(kExpectedData.ry.value(), unpacked_data.ry.value());
  EXPECT_EQ(kExpectedData.rz.value(), unpacked_data.rz.value());
}
