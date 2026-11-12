// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_test_macros.hpp>

#include "wpi/math/kinematics/DifferentialDriveWheelAccelerations.hpp"

using namespace wpi::math;

namespace {

using StructType =
    wpi::util::Struct<wpi::math::DifferentialDriveWheelAccelerations>;
const DifferentialDriveWheelAccelerations kExpectedData{
    DifferentialDriveWheelAccelerations{1.74_mps_sq, 35.04_mps_sq}};
}  // namespace

TEST_CASE("DifferentialDriveWheelAccelerationsStructTest Roundtrip",
          "[wpimath]") {
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  StructType::Pack(buffer, kExpectedData);

  DifferentialDriveWheelAccelerations unpacked_data =
      StructType::Unpack(buffer);

  CHECK(kExpectedData.left.value() == unpacked_data.left.value());
  CHECK(kExpectedData.right.value() == unpacked_data.right.value());
}
