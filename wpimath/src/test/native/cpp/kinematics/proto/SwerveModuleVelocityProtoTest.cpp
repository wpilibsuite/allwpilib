// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_test_macros.hpp>

#include "wpi/math/kinematics/SwerveModuleVelocity.hpp"
#include "wpi/util/SmallVector.hpp"

using namespace wpi::math;

namespace {

const SwerveModuleVelocity EXPECTED_DATA =
    SwerveModuleVelocity{22.9_mps, Rotation2d{3.3_rad}};
}  // namespace

TEST_CASE("SwerveModuleVelocityProtoTest Roundtrip", "[wpimath]") {
  wpi::util::ProtobufMessage<decltype(EXPECTED_DATA)> message;
  wpi::util::SmallVector<uint8_t, 64> buf;

  REQUIRE(message.Pack(buf, EXPECTED_DATA));
  auto unpacked_data = message.Unpack(buf);
  REQUIRE(unpacked_data.has_value());

  CHECK(EXPECTED_DATA.velocity.value() == unpacked_data->velocity.value());
  CHECK(EXPECTED_DATA.angle == unpacked_data->angle);
}
