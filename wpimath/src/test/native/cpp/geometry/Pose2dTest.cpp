/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <cmath>

#include "frc/geometry/Pose2d.h"
#include "gtest/gtest.h"

using namespace frc;

static constexpr double kEpsilon = 1E-9;

TEST(Pose2dTest, TransformBy) {
  const Pose2d initial{1_m, 2_m, Rotation2d(45.0_deg)};
  const Transform2d transform{Translation2d{5.0_m, 0.0_m}, Rotation2d(5.0_deg)};

  const auto transformed = initial + transform;

  EXPECT_NEAR(transformed.X().to<double>(), 1 + 5 / std::sqrt(2.0), kEpsilon);
  EXPECT_NEAR(transformed.Y().to<double>(), 2 + 5 / std::sqrt(2.0), kEpsilon);
  EXPECT_NEAR(transformed.Rotation().Degrees().to<double>(), 50.0, kEpsilon);
}

TEST(Pose2dTest, RelativeTo) {
  const Pose2d initial{0_m, 0_m, Rotation2d(45.0_deg)};
  const Pose2d final{5_m, 5_m, Rotation2d(45.0_deg)};

  const auto finalRelativeToInitial = final.RelativeTo(initial);

  EXPECT_NEAR(finalRelativeToInitial.X().to<double>(), 5.0 * std::sqrt(2.0),
              kEpsilon);
  EXPECT_NEAR(finalRelativeToInitial.Y().to<double>(), 0.0, kEpsilon);
  EXPECT_NEAR(finalRelativeToInitial.Rotation().Degrees().to<double>(), 0.0,
              kEpsilon);
}

TEST(Pose2dTest, Equality) {
  const Pose2d a{0_m, 5_m, Rotation2d(43_deg)};
  const Pose2d b{0_m, 5_m, Rotation2d(43_deg)};
  EXPECT_TRUE(a == b);
}

TEST(Pose2dTest, Inequality) {
  const Pose2d a{0_m, 5_m, Rotation2d(43_deg)};
  const Pose2d b{0_m, 5_ft, Rotation2d(43_deg)};
  EXPECT_TRUE(a != b);
}

TEST(Pose2dTest, Minus) {
  const Pose2d initial{0_m, 0_m, Rotation2d(45.0_deg)};
  const Pose2d final{5_m, 5_m, Rotation2d(45.0_deg)};

  const auto transform = final - initial;

  EXPECT_NEAR(transform.X().to<double>(), 5.0 * std::sqrt(2.0), kEpsilon);
  EXPECT_NEAR(transform.Y().to<double>(), 0.0, kEpsilon);
  EXPECT_NEAR(transform.Rotation().Degrees().to<double>(), 0.0, kEpsilon);
}
