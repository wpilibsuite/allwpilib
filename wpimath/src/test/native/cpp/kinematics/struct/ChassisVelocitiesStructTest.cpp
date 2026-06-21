// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/kinematics/ChassisVelocities.hpp"

using namespace wpi::math;

namespace {

using StructType = wpi::util::Struct<wpi::math::ChassisVelocities>;
const ChassisVelocities kExpectedData{
    ChassisVelocities{2.29_mps, 2.2_mps, 0.3504_rad_per_s}};
}  // namespace

TEST_CASE("ChassisVelocitiesStructTest Roundtrip", "[wpimath]") {
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  StructType::Pack(buffer, kExpectedData);

  ChassisVelocities unpacked_data = StructType::Unpack(buffer);

  CHECK(kExpectedData.vx.value() == unpacked_data.vx.value());
  CHECK(kExpectedData.vy.value() == unpacked_data.vy.value());
  CHECK(kExpectedData.omega.value() == unpacked_data.omega.value());
}
