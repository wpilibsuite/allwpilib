// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <iostream>
#include "frc/apriltag/AprilTagFieldLayout.h"
#include "frc/apriltag/AprilTagUtil.h"
#include "frc/geometry/Pose3d.h"
#include "frc/trajectory/TrajectoryConfig.h"
#include "frc/trajectory/TrajectoryUtil.h"
#include "gtest/gtest.h"

using namespace frc;

TEST(AprilTagJsonTest, DeserializeMatches) {
  auto layout = AprilTagFieldLayout{std::vector{
        AprilTagFieldLayout::AprilTag{1, Pose3d{}},
        AprilTagFieldLayout::AprilTag{3, Pose3d{0_m, 1_m, 0_m, Rotation3d{0_deg, 0_deg, 0_deg}}}}
    };

  AprilTagFieldLayout deserialized;
  EXPECT_NO_THROW(deserialized = AprilTagUtil::DeserializeAprilTagLayout(
                      AprilTagUtil::SerializeAprilTagLayout(layout)));
  EXPECT_EQ(layout.GetTags(), deserialized.GetTags());
}
