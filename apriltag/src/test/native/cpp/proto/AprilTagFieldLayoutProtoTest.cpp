// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <vector>

#include <gtest/gtest.h>

#include "wpi/apriltag/AprilTagFieldLayout.hpp"
#include "wpi/util/SmallVector.hpp"

using namespace wpi::apriltag;

namespace {

const AprilTagFieldLayout kExpectedData = AprilTagFieldLayout{
    std::vector{AprilTag{1, wpi::math::Pose3d{}},
                AprilTag{3, wpi::math::Pose3d{0_m, 1_m, 0_m,
                                              wpi::math::Rotation3d{
                                                  0_deg, 0_deg, 0_deg}}}},
    54_ft, 27_ft};
}  // namespace

TEST(AprilTagFieldLayoutProtoTest, Roundtrip) {
  wpi::util::ProtobufMessage<decltype(kExpectedData)> message;
  wpi::util::SmallVector<uint8_t, 256> buf;

  ASSERT_TRUE(message.Pack(buf, kExpectedData));
  auto unpacked_data = message.Unpack(buf);
  ASSERT_TRUE(unpacked_data.has_value());

  EXPECT_EQ(kExpectedData.GetFieldLength(), unpacked_data->GetFieldLength());
  EXPECT_EQ(kExpectedData.GetFieldWidth(), unpacked_data->GetFieldWidth());

  // Both lists are checked to confirm the presence of all tags and the absence
  // of extra tags

  for (AprilTag tag : kExpectedData.GetTags()) {
    EXPECT_EQ(tag.pose, unpacked_data->GetTagPose(tag.ID));
  }

  for (AprilTag tag : unpacked_data->GetTags()) {
    EXPECT_EQ(kExpectedData.GetTagPose(tag.ID), tag.pose);
  }
}
