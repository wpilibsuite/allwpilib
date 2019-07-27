/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
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
  const Pose2d initial{1.0, 2.0, Rotation2d::FromDegrees(45.0)};
  const Transform2d transform{Translation2d{5.0, 0.0},
                              Rotation2d::FromDegrees(5.0)};

  const auto transformed = initial + transform;

  EXPECT_NEAR(transformed.Translation().X(), 1 + 5 / std::sqrt(2.0), kEpsilon);
  EXPECT_NEAR(transformed.Translation().Y(), 2 + 5 / std::sqrt(2.0), kEpsilon);
  EXPECT_NEAR(transformed.Rotation().Degrees(), 50.0, kEpsilon);
}

TEST(Pose2dTest, RelativeTo) {
  const Pose2d initial{0.0, 0.0, Rotation2d::FromDegrees(45.0)};
  const Pose2d final{5.0, 5.0, Rotation2d::FromDegrees(45.0)};

  const auto finalRelativeToInitial = final.RelativeTo(initial);

  EXPECT_NEAR(finalRelativeToInitial.Translation().X(), 5.0 * std::sqrt(2.0),
              kEpsilon);
  EXPECT_NEAR(finalRelativeToInitial.Translation().Y(), 0.0, kEpsilon);
  EXPECT_NEAR(finalRelativeToInitial.Rotation().Degrees(), 0.0, kEpsilon);
}
