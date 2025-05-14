// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "frc/kinematics/DifferentialDriveWheelPositions.h"

using namespace frc;

namespace {

using StructType = wpi::Struct<frc::DifferentialDriveWheelPositions>;
const DifferentialDriveWheelPositions EXPECTED_DATA{
    DifferentialDriveWheelPositions{1.74_m, 35.04_m}};
}  // namespace

TEST(DifferentialDriveWheelPositionsStructTest, Roundtrip) {
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  StructType::Pack(buffer, EXPECTED_DATA);

  DifferentialDriveWheelPositions unpacked_data = StructType::Unpack(buffer);

  EXPECT_EQ(EXPECTED_DATA.left.value(), unpacked_data.left.value());
  EXPECT_EQ(EXPECTED_DATA.right.value(), unpacked_data.right.value());
}
