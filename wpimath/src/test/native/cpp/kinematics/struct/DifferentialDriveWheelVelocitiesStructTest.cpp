// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "wpi/math/kinematics/DifferentialDriveWheelVelocities.hpp"

using namespace wpi::math;

namespace {

using StructType =
    wpi::util::Struct<wpi::math::DifferentialDriveWheelVelocities>;
const DifferentialDriveWheelVelocities kExpectedData{
    DifferentialDriveWheelVelocities{1.74_mps, 35.04_mps}};
}  // namespace

TEST(DifferentialDriveWheelVelocitiesStructTest, Roundtrip) {
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  StructType::Pack(buffer, kExpectedData);

  DifferentialDriveWheelVelocities unpacked_data = StructType::Unpack(buffer);

  EXPECT_EQ(kExpectedData.left.value(), unpacked_data.left.value());
  EXPECT_EQ(kExpectedData.right.value(), unpacked_data.right.value());
}
