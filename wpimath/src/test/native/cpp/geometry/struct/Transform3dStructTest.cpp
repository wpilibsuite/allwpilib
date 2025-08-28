// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "wpimath/geometry/Transform3d.h"

using namespace wpimath;

namespace {

using StructType = wpi::Struct<wpimath::Transform3d>;
const Transform3d kExpectedData{
    Transform3d{Translation3d{0.3504_m, 22.9_m, 3.504_m},
                Rotation3d{Quaternion{0.3504, 35.04, 2.29, 0.3504}}}};
}  // namespace

TEST(Transform3dStructTest, Roundtrip) {
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  StructType::Pack(buffer, kExpectedData);

  Transform3d unpacked_data = StructType::Unpack(buffer);

  EXPECT_EQ(kExpectedData.Translation(), unpacked_data.Translation());
  EXPECT_EQ(kExpectedData.Rotation(), unpacked_data.Rotation());
}
