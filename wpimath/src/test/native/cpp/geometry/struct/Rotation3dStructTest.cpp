// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "wpimath/geometry/Rotation3d.h"

using namespace wpimath;

namespace {

using StructType = wpi::Struct<wpimath::Rotation3d>;
const Rotation3d kExpectedData{
    Rotation3d{Quaternion{2.29, 0.191, 0.191, 17.4}}};
}  // namespace

TEST(Rotation3dStructTest, Roundtrip) {
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  StructType::Pack(buffer, kExpectedData);

  Rotation3d unpacked_data = StructType::Unpack(buffer);

  EXPECT_EQ(kExpectedData.GetQuaternion(), unpacked_data.GetQuaternion());
}
