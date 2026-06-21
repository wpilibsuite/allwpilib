// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/kinematics/SwerveModuleVelocity.hpp"

using namespace wpi::math;

namespace {

using StructType = wpi::util::Struct<wpi::math::SwerveModuleVelocity>;
const SwerveModuleVelocity kExpectedData{
    SwerveModuleVelocity{22.9_mps, Rotation2d{3.3_rad}}};
}  // namespace

TEST_CASE("SwerveModuleVelocityStructTest Roundtrip", "[wpimath]") {
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  StructType::Pack(buffer, kExpectedData);

  SwerveModuleVelocity unpacked_data = StructType::Unpack(buffer);

  CHECK(kExpectedData.velocity.value() == unpacked_data.velocity.value());
  CHECK(kExpectedData.angle == unpacked_data.angle);
}
