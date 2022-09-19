// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <iostream>

#include "frc/apriltag/AprilTagUtil.h"
#include "frc/geometry/Pose3d.h"
#include "gtest/gtest.h"

using namespace frc;

TEST(AprilTagJsonTest, DeserializeMatches) {
  auto layout = std::vector{
      AprilTagUtil::AprilTag{1, Pose3d{}},
      AprilTagUtil::AprilTag{
          3, Pose3d{0_m, 1_m, 0_m, Rotation3d{0_deg, 0_deg, 0_deg}}}};

  std::vector<AprilTagUtil::AprilTag> deserialized;
  EXPECT_NO_THROW(deserialized = AprilTagUtil::DeserializeAprilTagLayout(
                      AprilTagUtil::SerializeAprilTagLayout(layout)));
  EXPECT_EQ(layout, deserialized);
}
