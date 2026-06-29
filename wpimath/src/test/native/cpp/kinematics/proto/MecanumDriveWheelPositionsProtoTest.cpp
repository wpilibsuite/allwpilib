// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/kinematics/MecanumDriveWheelPositions.hpp"
#include "wpi/util/SmallVector.hpp"

using namespace wpi::math;

namespace {

const MecanumDriveWheelPositions kExpectedData =
    MecanumDriveWheelPositions{17.4_m, 2.29_m, 22.9_m, 1.74_m};
}  // namespace

TEST_CASE("MecanumDriveWheelPositionsProtoTest Roundtrip", "[wpimath]") {
  wpi::util::ProtobufMessage<decltype(kExpectedData)> message;
  wpi::util::SmallVector<uint8_t, 64> buf;

  REQUIRE(message.Pack(buf, kExpectedData));
  auto unpacked_data = message.Unpack(buf);
  REQUIRE(unpacked_data.has_value());

  CHECK(kExpectedData.frontLeft.value() == unpacked_data->frontLeft.value());
  CHECK(kExpectedData.frontRight.value() == unpacked_data->frontRight.value());
  CHECK(kExpectedData.rearLeft.value() == unpacked_data->rearLeft.value());
  CHECK(kExpectedData.rearRight.value() == unpacked_data->rearRight.value());
}
