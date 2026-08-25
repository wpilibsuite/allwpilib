// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cstring>

#include <catch2/catch_test_macros.hpp>

#include "wpi/math/trajectory/DifferentialSample.hpp"

using namespace wpi::math;

namespace {

using StructType = wpi::util::Struct<DifferentialSample>;
const DifferentialSample EXPECTED_DATA{
    1.23_s,
    Pose2d{1.1_m, 2.2_m, Rotation2d{33.3_deg}},
    ChassisVelocities{3.3_mps, 4.4_mps, 5.5_rad_per_s},
    ChassisAccelerations{6.6_mps2, 7.7_mps2, 8.8_rad_per_s_sq},
    9.9_mps,
    10.1_mps};
}  // namespace

TEST_CASE("DifferentialSampleStructTest Roundtrip", "[wpimath]") {
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  StructType::Pack(buffer, EXPECTED_DATA);

  DifferentialSample unpacked_data = StructType::Unpack(buffer);

  CHECK(EXPECTED_DATA.time.value() == unpacked_data.time.value());
  CHECK(EXPECTED_DATA.pose == unpacked_data.pose);
  CHECK(EXPECTED_DATA.velocity.vx.value() == unpacked_data.velocity.vx.value());
  CHECK(EXPECTED_DATA.velocity.vy.value() == unpacked_data.velocity.vy.value());
  CHECK(EXPECTED_DATA.velocity.omega.value() ==
        unpacked_data.velocity.omega.value());
  CHECK(EXPECTED_DATA.acceleration.ax.value() ==
        unpacked_data.acceleration.ax.value());
  CHECK(EXPECTED_DATA.acceleration.ay.value() ==
        unpacked_data.acceleration.ay.value());
  CHECK(EXPECTED_DATA.acceleration.alpha.value() ==
        unpacked_data.acceleration.alpha.value());
  CHECK(EXPECTED_DATA.leftVelocity.value() ==
        unpacked_data.leftVelocity.value());
  CHECK(EXPECTED_DATA.rightVelocity.value() ==
        unpacked_data.rightVelocity.value());
}
