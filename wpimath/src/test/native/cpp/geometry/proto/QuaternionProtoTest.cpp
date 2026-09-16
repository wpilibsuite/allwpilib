// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_test_macros.hpp>

#include "wpi/math/geometry/Quaternion.hpp"
#include "wpi/util/SmallVector.hpp"

using namespace wpi::math;

namespace {

const Quaternion EXPECTED_DATA = Quaternion{1.1, 0.191, 35.04, 19.1};
}  // namespace

TEST_CASE("QuaternionProtoTest Roundtrip", "[wpimath]") {
  wpi::util::ProtobufMessage<decltype(EXPECTED_DATA)> message;
  wpi::util::SmallVector<uint8_t, 64> buf;

  REQUIRE(message.Pack(buf, EXPECTED_DATA));
  auto unpacked_data = message.Unpack(buf);
  REQUIRE(unpacked_data.has_value());

  CHECK(EXPECTED_DATA.W() == unpacked_data->W());
  CHECK(EXPECTED_DATA.X() == unpacked_data->X());
  CHECK(EXPECTED_DATA.Y() == unpacked_data->Y());
  CHECK(EXPECTED_DATA.Z() == unpacked_data->Z());
}
