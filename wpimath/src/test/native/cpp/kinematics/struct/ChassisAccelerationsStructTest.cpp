// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_test_macros.hpp>

#include "wpi/math/kinematics/ChassisAccelerations.hpp"

using namespace wpi::math;

namespace {

using StructType = wpi::util::Struct<wpi::math::ChassisAccelerations>;
const ChassisAccelerations kExpectedData{
    ChassisAccelerations{2.29_mps_sq, 2.2_mps_sq, 0.3504_rad_per_s_sq}};
}  // namespace

TEST_CASE("ChassisAccelerationsStructTest Roundtrip", "[wpimath]") {
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  StructType::Pack(buffer, kExpectedData);

  ChassisAccelerations unpacked_data = StructType::Unpack(buffer);

  CHECK(kExpectedData.ax.value() == unpacked_data.ax.value());
  CHECK(kExpectedData.ay.value() == unpacked_data.ay.value());
  CHECK(kExpectedData.alpha.value() == unpacked_data.alpha.value());
}
