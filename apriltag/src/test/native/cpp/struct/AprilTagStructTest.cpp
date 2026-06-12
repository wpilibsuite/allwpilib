// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "wpi/apriltag/AprilTag.hpp"

using namespace wpi::apriltag;

namespace {

using StructType = wpi::util::Struct<wpi::apriltag::AprilTag>;
const AprilTag kExpectedData =
    AprilTag{3, wpi::math::Pose3d{0_m, 1_m, 0_m,
                                  wpi::math::Rotation3d{0_deg, 0_deg, 0_deg}}};
}  // namespace

TEST(AprilTagStructTest, Roundtrip) {
  uint8_t buffer[StructType::GetSize()];
  std::memset(buffer, 0, StructType::GetSize());
  StructType::Pack(buffer, kExpectedData);

  AprilTag unpacked_data = StructType::Unpack(buffer);

  EXPECT_EQ(kExpectedData.ID, unpacked_data.ID);
  EXPECT_EQ(kExpectedData.pose, unpacked_data.pose);
}
