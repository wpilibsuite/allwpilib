// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_test_macros.hpp>

#include "wpi/math/geometry/Twist3d.hpp"
#include "wpi/util/SmallVector.hpp"

using namespace wpi::math;

namespace {

const Twist3d EXPECTED_DATA =
    Twist3d{1.1_m, 2.29_m, 35.04_m, 0.174_rad, 19.1_rad, 4.4_rad};
}  // namespace

TEST_CASE("Twist3dProtoTest Roundtrip", "[wpimath]") {
  wpi::util::ProtobufMessage<decltype(EXPECTED_DATA)> message;
  wpi::util::SmallVector<uint8_t, 64> buf;

  REQUIRE(message.Pack(buf, EXPECTED_DATA));
  auto unpacked_data = message.Unpack(buf);
  REQUIRE(unpacked_data.has_value());

  CHECK(EXPECTED_DATA.dx.value() == unpacked_data->dx.value());
  CHECK(EXPECTED_DATA.dy.value() == unpacked_data->dy.value());
  CHECK(EXPECTED_DATA.dz.value() == unpacked_data->dz.value());
  CHECK(EXPECTED_DATA.rx.value() == unpacked_data->rx.value());
  CHECK(EXPECTED_DATA.ry.value() == unpacked_data->ry.value());
  CHECK(EXPECTED_DATA.rz.value() == unpacked_data->rz.value());
}
