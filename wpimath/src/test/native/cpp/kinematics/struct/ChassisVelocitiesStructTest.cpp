// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "wpi/math/kinematics/ChassisVelocities.hpp"

using namespace wpi::math;

namespace {

using StructType = wpi::util::Struct<wpi::math::ChassisVelocities>;
const ChassisVelocities kExpectedData{
    ChassisVelocities{2.29_mps, 2.2_mps, 0.3504_rad_per_s}};
}  // namespace

TEST(ChassisVelocitiesStructTest, Roundtrip) {
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  StructType::Pack(buffer, kExpectedData);

  ChassisVelocities unpacked_data = StructType::Unpack(buffer);

  EXPECT_EQ(kExpectedData.vx.value(), unpacked_data.vx.value());
  EXPECT_EQ(kExpectedData.vy.value(), unpacked_data.vy.value());
  EXPECT_EQ(kExpectedData.omega.value(), unpacked_data.omega.value());
}
