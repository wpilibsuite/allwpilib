/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <cmath>

#include "frc/geometry/Pose2d.h"
#include "frc/geometry/Rotation2d.h"
#include "frc/geometry/Transform2d.h"
#include "frc/geometry/Translation2d.h"
#include "gtest/gtest.h"

using namespace frc;

static constexpr double kEpsilon = 1E-9;

TEST(Transform2dTest, Inverse) {
  const Pose2d initial{1_m, 2_m, Rotation2d(45.0_deg)};
  const Transform2d transform{Translation2d{5.0_m, 0.0_m}, Rotation2d(5.0_deg)};

  auto transformed = initial + transform;
  auto untransformed = transformed + transform.Inverse();

  EXPECT_NEAR(initial.X().to<double>(), untransformed.X().to<double>(),
              kEpsilon);
  EXPECT_NEAR(initial.Y().to<double>(), untransformed.Y().to<double>(),
              kEpsilon);
  EXPECT_NEAR(initial.Rotation().Degrees().to<double>(),
              untransformed.Rotation().Degrees().to<double>(), kEpsilon);
}
