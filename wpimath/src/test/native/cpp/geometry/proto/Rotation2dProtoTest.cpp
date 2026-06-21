// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/geometry/Rotation2d.hpp"
#include "wpi/util/SmallVector.hpp"

using namespace wpi::math;

namespace {

const Rotation2d kExpectedData = Rotation2d{1.91_rad};
}  // namespace

TEST_CASE("Rotation2dProtoTest Roundtrip", "[wpimath]") {
  wpi::util::ProtobufMessage<decltype(kExpectedData)> message;
  wpi::util::SmallVector<uint8_t, 64> buf;

  REQUIRE(message.Pack(buf, kExpectedData));
  auto unpacked_data = message.Unpack(buf);
  REQUIRE(unpacked_data.has_value());
  CHECK(kExpectedData.Radians().value() == unpacked_data->Radians().value());
}
