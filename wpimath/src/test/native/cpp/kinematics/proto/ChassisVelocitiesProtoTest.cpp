// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_test_macros.hpp>

#include "wpi/math/kinematics/ChassisVelocities.hpp"
#include "wpi/util/SmallVector.hpp"

using namespace wpi::math;

namespace {

const ChassisVelocities EXPECTED_DATA =
    ChassisVelocities{2.29_mps, 2.2_mps, 0.3504_rad_per_s};
}  // namespace

TEST_CASE("ChassisVelocitiesProtoTest Roundtrip", "[wpimath]") {
  wpi::util::ProtobufMessage<decltype(EXPECTED_DATA)> message;
  wpi::util::SmallVector<uint8_t, 64> buf;

  REQUIRE(message.Pack(buf, EXPECTED_DATA));
  auto unpacked_data = message.Unpack(buf);
  REQUIRE(unpacked_data.has_value());

  CHECK(EXPECTED_DATA.vx.value() == unpacked_data->vx.value());
  CHECK(EXPECTED_DATA.vy.value() == unpacked_data->vy.value());
  CHECK(EXPECTED_DATA.omega.value() == unpacked_data->omega.value());
}
