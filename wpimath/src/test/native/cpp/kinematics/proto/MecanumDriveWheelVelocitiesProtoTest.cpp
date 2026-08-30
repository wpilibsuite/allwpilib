// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_test_macros.hpp>

#include "wpi/math/kinematics/MecanumDriveWheelVelocities.hpp"
#include "wpi/util/SmallVector.hpp"

using namespace wpi::math;

namespace {

const MecanumDriveWheelVelocities EXPECTED_DATA =
    MecanumDriveWheelVelocities{2.29_mps, 17.4_mps, 4.4_mps, 0.229_mps};
}  // namespace

TEST_CASE("MecanumDriveWheelVelocitiesProtoTest Roundtrip", "[wpimath]") {
  wpi::util::ProtobufMessage<decltype(EXPECTED_DATA)> message;
  wpi::util::SmallVector<uint8_t, 64> buf;

  REQUIRE(message.Pack(buf, EXPECTED_DATA));
  auto unpacked_data = message.Unpack(buf);
  REQUIRE(unpacked_data.has_value());

  CHECK(EXPECTED_DATA.frontLeft.value() == unpacked_data->frontLeft.value());
  CHECK(EXPECTED_DATA.frontRight.value() == unpacked_data->frontRight.value());
  CHECK(EXPECTED_DATA.rearLeft.value() == unpacked_data->rearLeft.value());
  CHECK(EXPECTED_DATA.rearRight.value() == unpacked_data->rearRight.value());
}
