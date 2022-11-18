// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <vector>

#include <wpi/json.h>

#include "frc/apriltag/AprilTag.h"
#include "frc/apriltag/AprilTagFieldLayout.h"
#include "frc/geometry/Pose3d.h"
#include "gtest/gtest.h"

using namespace frc;

TEST(AprilTagPoseSetOriginTest, TransformationMatches) {
  auto layout = AprilTagFieldLayout{
      std::vector<AprilTag>{
          AprilTag{1,
                   Pose3d{0_ft, 0_ft, 0_ft, Rotation3d{0_deg, 0_deg, 0_deg}}},
          AprilTag{
              2, Pose3d{4_ft, 4_ft, 4_ft, Rotation3d{0_deg, 0_deg, 180_deg}}}},
      54_ft, 27_ft};

  layout.SetOrigin(
      AprilTagFieldLayout::OriginPosition::kRedAllianceWallRightSide);

  auto mirrorPose =
      Pose3d{54_ft, 27_ft, 0_ft, Rotation3d{0_deg, 0_deg, 180_deg}};
  EXPECT_EQ(mirrorPose, *layout.GetTagPose(1));
  mirrorPose = Pose3d{50_ft, 23_ft, 4_ft, Rotation3d{0_deg, 0_deg, 0_deg}};
  EXPECT_EQ(mirrorPose, *layout.GetTagPose(2));
}
