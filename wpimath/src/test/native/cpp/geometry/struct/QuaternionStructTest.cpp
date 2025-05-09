// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "frc/geometry/Quaternion.h"

using namespace frc;

namespace {

using StructType = wpi::Struct<frc::Quaternion>;
const Quaternion EXPECTED_DATA{Quaternion{1.1, 0.191, 35.04, 19.1}};
}  // namespace

TEST(QuaternionStructTest, Roundtrip) {
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  StructType::Pack(buffer, EXPECTED_DATA);

  Quaternion unpacked_data = StructType::Unpack(buffer);

  EXPECT_EQ(EXPECTED_DATA.W(), unpacked_data.W());
  EXPECT_EQ(EXPECTED_DATA.X(), unpacked_data.X());
  EXPECT_EQ(EXPECTED_DATA.Y(), unpacked_data.Y());
  EXPECT_EQ(EXPECTED_DATA.Z(), unpacked_data.Z());
}
