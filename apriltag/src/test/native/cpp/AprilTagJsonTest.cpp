// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <vector>

#include <gtest/gtest.h>

#include "wpi/apriltag/AprilTag.hpp"
#include "wpi/apriltag/AprilTagFieldLayout.hpp"
#include "wpi/math/geometry/Pose3d.hpp"
#include "wpi/util/json.hpp"

using namespace wpi::apriltag;

TEST(AprilTagJsonTest, DeserializeMatches) {
  auto layout = AprilTagFieldLayout{
      std::vector{
          AprilTag{1, wpi::math::Pose3d{}},
          AprilTag{3, wpi::math::Pose3d{0_m, 1_m, 0_m, wpi::math::Rotation3d{0_deg, 0_deg, 0_deg}}}},
      54_ft, 27_ft};

  AprilTagFieldLayout deserialized;
  wpi::util::json json = layout;
  EXPECT_NO_THROW(deserialized = json.get<AprilTagFieldLayout>());
  EXPECT_EQ(layout, deserialized);
}
