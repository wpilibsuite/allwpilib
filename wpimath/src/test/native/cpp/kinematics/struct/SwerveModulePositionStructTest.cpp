// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_test_macros.hpp>

#include "wpi/math/kinematics/SwerveModulePosition.hpp"

using namespace wpi::math;

namespace {

using StructType = wpi::util::Struct<wpi::math::SwerveModulePosition>;
const SwerveModulePosition EXPECTED_DATA{
    SwerveModulePosition{3.504_m, Rotation2d{17.4_rad}}};
}  // namespace

TEST_CASE("SwerveModulePositionStructTest Roundtrip", "[wpimath]") {
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  StructType::Pack(buffer, EXPECTED_DATA);

  SwerveModulePosition unpacked_data = StructType::Unpack(buffer);

  CHECK(EXPECTED_DATA.distance.value() == unpacked_data.distance.value());
  CHECK(EXPECTED_DATA.angle == unpacked_data.angle);
}
