// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/geometry/Twist2d.hpp"
#include "wpi/util/SmallVector.hpp"

using namespace wpi::math;

namespace {

const Twist2d kExpectedData = Twist2d{2.29_m, 35.04_m, 35.04_rad};
}  // namespace

TEST_CASE("Twist2dProtoTest Roundtrip", "[wpimath]") {
  wpi::util::ProtobufMessage<decltype(kExpectedData)> message;
  wpi::util::SmallVector<uint8_t, 64> buf;

  REQUIRE(message.Pack(buf, kExpectedData));
  auto unpacked_data = message.Unpack(buf);
  REQUIRE(unpacked_data.has_value());

  CHECK(kExpectedData.dx.value() == unpacked_data->dx.value());
  CHECK(kExpectedData.dy.value() == unpacked_data->dy.value());
  CHECK(kExpectedData.dtheta.value() == unpacked_data->dtheta.value());
}
