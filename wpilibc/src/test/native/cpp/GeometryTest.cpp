/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/geometry/Transform2d.h"
#include "frc/geometry/Pose2d.h"
#include "gtest/gtest.h"

TEST(GeometryTest, TransformTests) {
  const frc::Pose2d initial{
      0.0, 0.0, frc::Rotation2d::FromDegrees(45.0)};
  const frc::Pose2d final{
      5.0, 5.0, frc::Rotation2d::FromDegrees(45.0)};
  const auto finalRelativeToInitial = final.RelativeTo(initial);

  EXPECT_EQ(finalRelativeToInitial.Translation().X(), 5.0 * std::sqrt(2.0));
  EXPECT_EQ(finalRelativeToInitial.Translation().Y(), 0.0);
  EXPECT_EQ(finalRelativeToInitial.Rotation().Degrees(), 0.0);

}