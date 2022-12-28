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

TEST(AprilTagJsonTest, DeserializeMatches) {
  auto layout = AprilTagFieldLayout{
      std::vector{
          AprilTag{1, Pose3d{}},
          AprilTag{3, Pose3d{0_m, 1_m, 0_m, Rotation3d{0_deg, 0_deg, 0_deg}}}},
      54_ft, 27_ft};

  AprilTagFieldLayout deserialized;
  wpi::json json = layout;
  EXPECT_NO_THROW(deserialized = json.get<AprilTagFieldLayout>());
  EXPECT_EQ(layout, deserialized);
}
