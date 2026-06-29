// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/util/SmallVector.hpp"

using namespace wpi::math;

namespace {

const Pose2d kExpectedData =
    Pose2d{Translation2d{0.191_m, 2.2_m}, Rotation2d{22.9_rad}};
}  // namespace

TEST_CASE("Pose2dProtoTest Roundtrip", "[wpimath]") {
  wpi::util::ProtobufMessage<decltype(kExpectedData)> message;
  wpi::util::SmallVector<uint8_t, 64> buf;

  REQUIRE(message.Pack(buf, kExpectedData));
  auto unpacked_data = message.Unpack(buf);
  REQUIRE(unpacked_data.has_value());

  CHECK(kExpectedData.Translation() == unpacked_data->Translation());
  CHECK(kExpectedData.Rotation() == unpacked_data->Rotation());
}
