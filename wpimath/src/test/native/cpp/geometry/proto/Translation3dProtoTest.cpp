// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/geometry/Translation3d.hpp"
#include "wpi/util/SmallVector.hpp"

using namespace wpi::math;

namespace {

const Translation3d kExpectedData = Translation3d{35.04_m, 22.9_m, 3.504_m};
}  // namespace

TEST_CASE("Translation3dProtoTest Roundtrip", "[wpimath]") {
  wpi::util::ProtobufMessage<decltype(kExpectedData)> message;
  wpi::util::SmallVector<uint8_t, 64> buf;

  REQUIRE(message.Pack(buf, kExpectedData));
  auto unpacked_data = message.Unpack(buf);
  REQUIRE(unpacked_data.has_value());

  CHECK(kExpectedData.X() == unpacked_data->X());
  CHECK(kExpectedData.Y() == unpacked_data->Y());
  CHECK(kExpectedData.Z() == unpacked_data->Z());
}
