// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "wpi/apriltag/AprilTag.hpp"
#include "wpi/util/SmallVector.hpp"

using namespace wpi::apriltag;

namespace {

const AprilTag kExpectedData =
    AprilTag{3, wpi::math::Pose3d{0_m, 1_m, 0_m,
                                  wpi::math::Rotation3d{0_deg, 0_deg, 0_deg}}};
}  // namespace

TEST(AprilTagProtoTest, Roundtrip) {
  wpi::util::ProtobufMessage<decltype(kExpectedData)> message;
  wpi::util::SmallVector<uint8_t, 64> buf;

  ASSERT_TRUE(message.Pack(buf, kExpectedData));
  auto unpacked_data = message.Unpack(buf);
  ASSERT_TRUE(unpacked_data.has_value());

  EXPECT_EQ(kExpectedData.ID, unpacked_data->ID);
  EXPECT_EQ(kExpectedData.pose, unpacked_data->pose);
}
