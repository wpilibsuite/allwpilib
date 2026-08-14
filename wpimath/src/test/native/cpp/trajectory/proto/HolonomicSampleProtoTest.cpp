// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_test_macros.hpp>

#include "wpi/math/trajectory/HolonomicSample.hpp"
#include "wpi/util/SmallVector.hpp"

using namespace wpi::math;

namespace {
const HolonomicSample kExpectedData =
    HolonomicSample{1.5_s, Pose2d{1.1_m, 2.2_m, 30_deg},
                    ChassisVelocities{1_mps, 2_mps, 3_rad_per_s},
                    ChassisAccelerations{4_mps_sq, 5_mps_sq, 6_rad_per_s_sq}};
}  // namespace

TEST_CASE("HolonomicSampleProtoTest Roundtrip", "[wpimath]") {
  wpi::util::ProtobufMessage<decltype(kExpectedData)> message;
  wpi::util::SmallVector<uint8_t, 64> buf;

  REQUIRE(message.Pack(buf, kExpectedData));
  auto unpacked_data = message.Unpack(buf);
  REQUIRE(unpacked_data.has_value());

  CHECK(kExpectedData == unpacked_data.value());
}
