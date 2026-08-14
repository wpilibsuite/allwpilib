// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_test_macros.hpp>

#include "wpi/math/trajectory/TrajectorySample.hpp"
#include "wpi/util/SmallVector.hpp"

using namespace wpi::math;

namespace {
const TrajectorySample kExpectedData = TrajectorySample{1.5_s};
}  // namespace

TEST_CASE("TrajectorySampleProtoTest Roundtrip", "[wpimath]") {
  wpi::util::ProtobufMessage<decltype(kExpectedData)> message;
  wpi::util::SmallVector<uint8_t, 64> buf;

  REQUIRE(message.Pack(buf, kExpectedData));
  auto unpacked_data = message.Unpack(buf);
  REQUIRE(unpacked_data.has_value());

  CHECK(kExpectedData == unpacked_data.value());
}
