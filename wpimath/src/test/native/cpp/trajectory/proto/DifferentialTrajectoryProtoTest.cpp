// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "wpi/math/trajectory/DifferentialTrajectory.hpp"
#include "wpi/util/SmallVector.hpp"

using namespace wpi::math;

namespace {
const DifferentialTrajectory kExpectedData =
    DifferentialTrajectory{std::vector<DifferentialSample>{
        DifferentialSample{
            0_s, Pose2d{}, ChassisVelocities{1_mps, 0_mps, 0_rad_per_s},
            ChassisAccelerations{0.5_mps_sq, 0_mps_sq, 0_rad_per_s_sq}, 0.9_mps,
            1.1_mps},
        DifferentialSample{
            1_s, Pose2d{1.1_m, 2.2_m, 30_deg},
            ChassisVelocities{1.5_mps, 0_mps, 0.3_rad_per_s},
            ChassisAccelerations{0.1_mps_sq, 0_mps_sq, 0.4_rad_per_s_sq},
            1.4_mps, 1.6_mps}}};
}  // namespace

TEST_CASE("DifferentialTrajectoryProtoTest Roundtrip", "[wpimath]") {
  wpi::util::ProtobufMessage<decltype(kExpectedData)> message;
  wpi::util::SmallVector<uint8_t, 64> buf;

  REQUIRE(message.Pack(buf, kExpectedData));
  auto unpacked_data = message.Unpack(buf);
  REQUIRE(unpacked_data.has_value());

  CHECK(kExpectedData == unpacked_data.value());
}
