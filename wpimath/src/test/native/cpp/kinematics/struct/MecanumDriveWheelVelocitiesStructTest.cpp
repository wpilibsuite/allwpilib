// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/kinematics/MecanumDriveWheelVelocities.hpp"

using namespace wpi::math;

namespace {

using StructType = wpi::util::Struct<wpi::math::MecanumDriveWheelVelocities>;
const MecanumDriveWheelVelocities kExpectedData{
    MecanumDriveWheelVelocities{2.29_mps, 17.4_mps, 4.4_mps, 0.229_mps}};
}  // namespace

TEST_CASE("MecanumDriveWheelVelocitiesStructTest Roundtrip", "[wpimath]") {
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  StructType::Pack(buffer, kExpectedData);

  MecanumDriveWheelVelocities unpacked_data = StructType::Unpack(buffer);

  CHECK(kExpectedData.frontLeft.value() == unpacked_data.frontLeft.value());
  CHECK(kExpectedData.frontRight.value() == unpacked_data.frontRight.value());
  CHECK(kExpectedData.rearLeft.value() == unpacked_data.rearLeft.value());
  CHECK(kExpectedData.rearRight.value() == unpacked_data.rearRight.value());
}
