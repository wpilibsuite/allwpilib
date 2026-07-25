// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <vector>

#include <gtest/gtest.h>

#include "wpi/math/trajectory/HolonomicTrajectory.hpp"
#include "wpi/util/SmallVector.hpp"

using namespace wpi::math;

namespace {
const HolonomicTrajectory kExpectedData =
    HolonomicTrajectory{std::vector<HolonomicSample>{
        HolonomicSample{
            0_s, Pose2d{}, ChassisVelocities{1_mps, 0_mps, 0_rad_per_s},
            ChassisAccelerations{0.5_mps_sq, 0_mps_sq, 0_rad_per_s_sq}},
        HolonomicSample{
            1_s, Pose2d{1.1_m, 2.2_m, 30_deg},
            ChassisVelocities{1.5_mps, 2_mps, 0.3_rad_per_s},
            ChassisAccelerations{0.1_mps_sq, 0.2_mps_sq, 0.4_rad_per_s_sq}}}};
}  // namespace

TEST(HolonomicTrajectoryProtoTest, Roundtrip) {
  wpi::util::ProtobufMessage<decltype(kExpectedData)> message;
  wpi::util::SmallVector<uint8_t, 64> buf;

  ASSERT_TRUE(message.Pack(buf, kExpectedData));
  auto unpacked_data = message.Unpack(buf);
  ASSERT_TRUE(unpacked_data.has_value());

  EXPECT_EQ(kExpectedData, unpacked_data.value());
}
