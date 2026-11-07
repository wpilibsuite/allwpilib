// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_test_macros.hpp>

#include "wpi/math/kinematics/ChassisVelocities.hpp"

using namespace wpi::math;

namespace {

using StructType = wpi::util::Struct<wpi::math::ChassisVelocities>;
const ChassisVelocities EXPECTED_DATA{
    ChassisVelocities{2.29_mps, 2.2_mps, 0.3504_rad_per_s}};
}  // namespace

TEST_CASE("ChassisVelocitiesStructTest Roundtrip", "[wpimath]") {
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  StructType::Pack(buffer, EXPECTED_DATA);

  ChassisVelocities unpacked_data = StructType::Unpack(buffer);

  CHECK(EXPECTED_DATA.vx.value() == unpacked_data.vx.value());
  CHECK(EXPECTED_DATA.vy.value() == unpacked_data.vy.value());
  CHECK(EXPECTED_DATA.omega.value() == unpacked_data.omega.value());
}
