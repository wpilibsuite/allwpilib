// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/kinematics/SwerveModulePosition.hpp"

using namespace wpi::math;

namespace {

using StructType = wpi::util::Struct<wpi::math::SwerveModulePosition>;
const SwerveModulePosition kExpectedData{
    SwerveModulePosition{3.504_m, Rotation2d{17.4_rad}}};
}  // namespace

TEST_CASE("SwerveModulePositionStructTest Roundtrip", "[wpimath]") {
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  StructType::Pack(buffer, kExpectedData);

  SwerveModulePosition unpacked_data = StructType::Unpack(buffer);

  CHECK(kExpectedData.distance.value() == unpacked_data.distance.value());
  CHECK(kExpectedData.angle == unpacked_data.angle);
}
