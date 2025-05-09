// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "frc/kinematics/DifferentialDriveWheelSpeeds.h"

using namespace frc;

namespace {

using StructType = wpi::Struct<frc::DifferentialDriveWheelSpeeds>;
const DifferentialDriveWheelSpeeds EXPECTED_DATA{
    DifferentialDriveWheelSpeeds{1.74_mps, 35.04_mps}};
}  // namespace

TEST(DifferentialDriveWheelSpeedsStructTest, Roundtrip) {
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  StructType::Pack(buffer, EXPECTED_DATA);

  DifferentialDriveWheelSpeeds unpacked_data = StructType::Unpack(buffer);

  EXPECT_EQ(EXPECTED_DATA.left.value(), unpacked_data.left.value());
  EXPECT_EQ(EXPECTED_DATA.right.value(), unpacked_data.right.value());
}
