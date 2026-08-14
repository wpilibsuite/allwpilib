// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cstring>

#include <catch2/catch_test_macros.hpp>

#include "wpi/math/trajectory/HolonomicSample.hpp"

using namespace wpi::math;

namespace {

using StructType = wpi::util::Struct<HolonomicSample>;
const HolonomicSample kExpectedData{
    1.23_s, Pose2d{1.1_m, 2.2_m, Rotation2d{33.3_deg}},
    ChassisVelocities{3.3_mps, 4.4_mps, 5.5_rad_per_s},
    ChassisAccelerations{6.6_mps_sq, 7.7_mps_sq, 8.8_rad_per_s_sq}};
}  // namespace

TEST_CASE("HolonomicSampleStructTest Roundtrip", "[wpimath]") {
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  StructType::Pack(buffer, kExpectedData);

  HolonomicSample unpacked_data = StructType::Unpack(buffer);

  CHECK(kExpectedData.time.value() == unpacked_data.time.value());
  CHECK(kExpectedData.pose == unpacked_data.pose);
  CHECK(kExpectedData.velocity.vx.value() == unpacked_data.velocity.vx.value());
  CHECK(kExpectedData.velocity.vy.value() == unpacked_data.velocity.vy.value());
  CHECK(kExpectedData.velocity.omega.value() ==
        unpacked_data.velocity.omega.value());
  CHECK(kExpectedData.acceleration.ax.value() ==
        unpacked_data.acceleration.ax.value());
  CHECK(kExpectedData.acceleration.ay.value() ==
        unpacked_data.acceleration.ay.value());
  CHECK(kExpectedData.acceleration.alpha.value() ==
        unpacked_data.acceleration.alpha.value());
}
