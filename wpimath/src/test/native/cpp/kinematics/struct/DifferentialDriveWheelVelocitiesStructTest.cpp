// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_test_macros.hpp>

#include "wpi/math/kinematics/DifferentialDriveWheelVelocities.hpp"

using namespace wpi::math;

namespace {

using StructType =
    wpi::util::Struct<wpi::math::DifferentialDriveWheelVelocities>;
const DifferentialDriveWheelVelocities EXPECTED_DATA{
    DifferentialDriveWheelVelocities{1.74_mps, 35.04_mps}};
}  // namespace

TEST_CASE("DifferentialDriveWheelVelocitiesStructTest Roundtrip", "[wpimath]") {
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  StructType::Pack(buffer, EXPECTED_DATA);

  DifferentialDriveWheelVelocities unpacked_data = StructType::Unpack(buffer);

  CHECK(EXPECTED_DATA.left.value() == unpacked_data.left.value());
  CHECK(EXPECTED_DATA.right.value() == unpacked_data.right.value());
}
