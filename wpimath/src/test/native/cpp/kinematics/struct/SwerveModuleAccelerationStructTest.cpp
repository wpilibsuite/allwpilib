// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_test_macros.hpp>

#include "wpi/math/kinematics/SwerveModuleAcceleration.hpp"

using namespace wpi::math;

namespace {

using StructType = wpi::util::Struct<wpi::math::SwerveModuleAcceleration>;
const SwerveModuleAcceleration kExpectedData{
    SwerveModuleAcceleration{22.9_mps_sq, Rotation2d{3.3_rad}}};
}  // namespace

TEST_CASE("SwerveModuleAccelerationStructTest Roundtrip", "[wpimath]") {
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  StructType::Pack(buffer, kExpectedData);

  SwerveModuleAcceleration unpacked_data = StructType::Unpack(buffer);

  CHECK(kExpectedData.acceleration.value() ==
        unpacked_data.acceleration.value());
  CHECK(kExpectedData.angle == unpacked_data.angle);
}
