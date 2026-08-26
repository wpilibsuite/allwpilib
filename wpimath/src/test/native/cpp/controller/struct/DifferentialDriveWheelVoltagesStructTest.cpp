// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_test_macros.hpp>

#include "wpi/math/controller/DifferentialDriveWheelVoltages.hpp"

using namespace wpi::math;

namespace {

using StructType = wpi::util::Struct<wpi::math::DifferentialDriveWheelVoltages>;
const DifferentialDriveWheelVoltages EXPECTED_DATA{
    DifferentialDriveWheelVoltages{0.174_V, 0.191_V}};
}  // namespace

TEST_CASE("DifferentialDriveWheelVoltagesStructTest Roundtrip", "[wpimath]") {
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  StructType::Pack(buffer, EXPECTED_DATA);

  DifferentialDriveWheelVoltages unpacked_data = StructType::Unpack(buffer);

  CHECK(EXPECTED_DATA.left.value() == unpacked_data.left.value());
  CHECK(EXPECTED_DATA.right.value() == unpacked_data.right.value());
}
