// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "wpi/math/trajectory/DifferentialSample.hpp"

using namespace wpi::math;

namespace {

using StructType = wpi::util::Struct<DifferentialSample>;
const DifferentialSample kExpectedData{
    1.23_s,
    Pose2d{1.1_m, 2.2_m, Rotation2d{33.3_deg}},
    ChassisVelocities{3.3_mps, 4.4_mps, 5.5_rad_per_s},
    ChassisAccelerations{6.6_mps_sq, 7.7_mps_sq, 8.8_rad_per_s_sq},
    9.9_mps,
    10.1_mps};
}  // namespace

TEST(DifferentialSampleStructTest, Roundtrip) {
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  StructType::Pack(buffer, kExpectedData);

  DifferentialSample unpacked_data = StructType::Unpack(buffer);

  EXPECT_EQ(kExpectedData.time.value(), unpacked_data.time.value());
  EXPECT_EQ(kExpectedData.pose, unpacked_data.pose);
  EXPECT_EQ(kExpectedData.velocity.vx.value(),
            unpacked_data.velocity.vx.value());
  EXPECT_EQ(kExpectedData.velocity.vy.value(),
            unpacked_data.velocity.vy.value());
  EXPECT_EQ(kExpectedData.velocity.omega.value(),
            unpacked_data.velocity.omega.value());
  EXPECT_EQ(kExpectedData.acceleration.ax.value(),
            unpacked_data.acceleration.ax.value());
  EXPECT_EQ(kExpectedData.acceleration.ay.value(),
            unpacked_data.acceleration.ay.value());
  EXPECT_EQ(kExpectedData.acceleration.alpha.value(),
            unpacked_data.acceleration.alpha.value());
  EXPECT_EQ(kExpectedData.leftVelocity.value(),
            unpacked_data.leftVelocity.value());
  EXPECT_EQ(kExpectedData.rightVelocity.value(),
            unpacked_data.rightVelocity.value());
}
