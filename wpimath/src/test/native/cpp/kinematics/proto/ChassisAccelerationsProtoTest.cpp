// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_test_macros.hpp>

#include "wpi/math/kinematics/ChassisAccelerations.hpp"
#include "wpi/util/SmallVector.hpp"

using namespace wpi::math;

namespace {

const ChassisAccelerations kExpectedData =
    ChassisAccelerations{2.29_mps_sq, 2.2_mps_sq, 0.3504_rad_per_s_sq};
}  // namespace

TEST_CASE("ChassisAccelerationsProtoTest Roundtrip", "[wpimath]") {
  wpi::util::ProtobufMessage<decltype(kExpectedData)> message;
  wpi::util::SmallVector<uint8_t, 64> buf;

  REQUIRE(message.Pack(buf, kExpectedData));
  auto unpacked_data = message.Unpack(buf);
  REQUIRE(unpacked_data.has_value());

  CHECK(kExpectedData.ax.value() == unpacked_data->ax.value());
  CHECK(kExpectedData.ay.value() == unpacked_data->ay.value());
  CHECK(kExpectedData.alpha.value() == unpacked_data->alpha.value());
}
