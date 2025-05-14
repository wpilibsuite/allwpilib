// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "frc/geometry/Pose3d.h"

using namespace frc;

namespace {

using StructType = wpi::Struct<frc::Pose3d>;
const Pose3d EXPECTED_DATA{
    Pose3d{Translation3d{1.1_m, 2.2_m, 1.1_m},
           Rotation3d{Quaternion{1.91, 0.3504, 3.3, 1.74}}}};
}  // namespace

TEST(Pose3dStructTest, Roundtrip) {
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  StructType::Pack(buffer, EXPECTED_DATA);

  Pose3d unpacked_data = StructType::Unpack(buffer);

  EXPECT_EQ(EXPECTED_DATA.Translation(), unpacked_data.Translation());
  EXPECT_EQ(EXPECTED_DATA.Rotation(), unpacked_data.Rotation());
}
