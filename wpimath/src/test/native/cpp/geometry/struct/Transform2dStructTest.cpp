// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "wpi/math/geometry/Transform2d.hpp"

using namespace frc;

namespace {

using StructType = wpi::Struct<frc::Transform2d>;
const Transform2d kExpectedData{
    Transform2d{Translation2d{0.191_m, 2.2_m}, Rotation2d{4.4_rad}}};
}  // namespace

TEST(Transform2dStructTest, Roundtrip) {
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  StructType::Pack(buffer, kExpectedData);

  Transform2d unpacked_data = StructType::Unpack(buffer);

  EXPECT_EQ(kExpectedData.Translation(), unpacked_data.Translation());
  EXPECT_EQ(kExpectedData.Rotation(), unpacked_data.Rotation());
}
