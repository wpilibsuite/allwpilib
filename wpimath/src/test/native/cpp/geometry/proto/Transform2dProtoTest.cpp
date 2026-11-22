// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_test_macros.hpp>

#include "wpi/math/geometry/Transform2d.hpp"
#include "wpi/util/SmallVector.hpp"

using namespace wpi::math;

namespace {

const Transform2d EXPECTED_DATA =
    Transform2d{Translation2d{0.191_m, 2.2_m}, Rotation2d{4.4_rad}};
}  // namespace

TEST_CASE("Transform2dProtoTest Roundtrip", "[wpimath]") {
  wpi::util::ProtobufMessage<decltype(EXPECTED_DATA)> message;
  wpi::util::SmallVector<uint8_t, 64> buf;

  REQUIRE(message.Pack(buf, EXPECTED_DATA));
  auto unpacked_data = message.Unpack(buf);
  REQUIRE(unpacked_data.has_value());

  CHECK(EXPECTED_DATA.Translation() == unpacked_data->Translation());
  CHECK(EXPECTED_DATA.Rotation() == unpacked_data->Rotation());
}
