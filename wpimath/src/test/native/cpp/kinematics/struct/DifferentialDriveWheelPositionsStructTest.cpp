// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_test_macros.hpp>

#include "wpi/math/kinematics/DifferentialDriveWheelPositions.hpp"

using namespace wpi::math;

namespace {

using StructType =
    wpi::util::Struct<wpi::math::DifferentialDriveWheelPositions>;
const DifferentialDriveWheelPositions EXPECTED_DATA{
    DifferentialDriveWheelPositions{1.74_m, 35.04_m}};
}  // namespace

TEST_CASE("DifferentialDriveWheelPositionsStructTest Roundtrip", "[wpimath]") {
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  StructType::Pack(buffer, EXPECTED_DATA);

  DifferentialDriveWheelPositions unpacked_data = StructType::Unpack(buffer);

  CHECK(EXPECTED_DATA.left.value() == unpacked_data.left.value());
  CHECK(EXPECTED_DATA.right.value() == unpacked_data.right.value());
}
