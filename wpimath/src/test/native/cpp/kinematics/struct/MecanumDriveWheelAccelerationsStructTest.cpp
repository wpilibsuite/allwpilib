// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_test_macros.hpp>

#include "wpi/math/kinematics/MecanumDriveWheelAccelerations.hpp"

using namespace wpi::math;

namespace {

using StructType = wpi::util::Struct<wpi::math::MecanumDriveWheelAccelerations>;
const MecanumDriveWheelAccelerations kExpectedData{
    MecanumDriveWheelAccelerations{2.29_mps_sq, 17.4_mps_sq, 4.4_mps_sq,
                                   0.229_mps_sq}};
}  // namespace

TEST_CASE("MecanumDriveWheelAccelerationsStructTest Roundtrip", "[wpimath]") {
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  StructType::Pack(buffer, kExpectedData);

  MecanumDriveWheelAccelerations unpacked_data = StructType::Unpack(buffer);

  CHECK(kExpectedData.frontLeft.value() == unpacked_data.frontLeft.value());
  CHECK(kExpectedData.frontRight.value() == unpacked_data.frontRight.value());
  CHECK(kExpectedData.rearLeft.value() == unpacked_data.rearLeft.value());
  CHECK(kExpectedData.rearRight.value() == unpacked_data.rearRight.value());
}
