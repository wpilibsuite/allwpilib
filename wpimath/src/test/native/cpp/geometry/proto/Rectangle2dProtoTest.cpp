// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/geometry/Rectangle2d.hpp"
#include "wpi/util/SmallVector.hpp"

using namespace wpi::math;

namespace {

const Rectangle2d kExpectedData{
    Pose2d{Translation2d{0.191_m, 2.2_m}, Rotation2d{22.9_rad}}, 1.2_m, 2.3_m};
}  // namespace

TEST_CASE("Rectangle2dProtoTest Roundtrip", "[wpimath]") {
  wpi::util::ProtobufMessage<decltype(kExpectedData)> message;
  wpi::util::SmallVector<uint8_t, 64> buf;

  REQUIRE(message.Pack(buf, kExpectedData));
  auto unpacked_data = message.Unpack(buf);
  REQUIRE(unpacked_data.has_value());

  CHECK(kExpectedData.Center() == unpacked_data->Center());
  CHECK(kExpectedData.XWidth() == unpacked_data->XWidth());
  CHECK(kExpectedData.YWidth() == unpacked_data->YWidth());
}
