// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "frc/geometry/Translation3d.h"

using namespace frc;

namespace {

using StructType = wpi::Struct<frc::Translation3d>;
const Translation3d EXPECTED_DATA{Translation3d{35.04_m, 22.9_m, 3.504_m}};
}  // namespace

TEST(Translation3dStructTest, Roundtrip) {
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  StructType::Pack(buffer, EXPECTED_DATA);

  Translation3d unpacked_data = StructType::Unpack(buffer);

  EXPECT_EQ(EXPECTED_DATA.X(), unpacked_data.X());
  EXPECT_EQ(EXPECTED_DATA.Y(), unpacked_data.Y());
  EXPECT_EQ(EXPECTED_DATA.Z(), unpacked_data.Z());
}
