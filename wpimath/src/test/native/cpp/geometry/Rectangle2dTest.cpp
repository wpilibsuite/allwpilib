// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>
#include <wpi/math/geometry/Rectangle2d.h>

TEST(Rectangle2dTest, NewWithCorners) {
  constexpr wpi::math::Translation2d cornerA{1_m, 2_m};
  constexpr wpi::math::Translation2d cornerB{4_m, 6_m};

  wpi::math::Rectangle2d rect{cornerA, cornerB};

  EXPECT_EQ(3.0, rect.XWidth().value());
  EXPECT_EQ(4.0, rect.YWidth().value());
  EXPECT_EQ(2.5, rect.Center().X().value());
  EXPECT_EQ(4.0, rect.Center().Y().value());
}

TEST(Rectangle2dTest, Intersects) {
  constexpr wpi::math::Pose2d center{4_m, 3_m, 90_deg};
  constexpr wpi::math::Rectangle2d rect{center, 2_m, 3_m};

  EXPECT_TRUE(rect.Intersects(wpi::math::Translation2d{5.5_m, 4_m}));
  EXPECT_TRUE(rect.Intersects(wpi::math::Translation2d{3_m, 2_m}));
  EXPECT_FALSE(rect.Intersects(wpi::math::Translation2d{4_m, 1.5_m}));
  EXPECT_FALSE(rect.Intersects(wpi::math::Translation2d{4_m, 3.5_m}));
}

TEST(Rectangle2dTest, Contains) {
  constexpr wpi::math::Pose2d center{2_m, 3_m, 45_deg};
  constexpr wpi::math::Rectangle2d rect{center, 3_m, 1_m};

  EXPECT_TRUE(rect.Contains(wpi::math::Translation2d{2_m, 3_m}));
  EXPECT_TRUE(rect.Contains(wpi::math::Translation2d{3_m, 4_m}));
  EXPECT_FALSE(rect.Contains(wpi::math::Translation2d{3_m, 3_m}));
}

TEST(Rectangle2dTest, Distance) {
  constexpr double kEpsilon = 1E-9;

  constexpr wpi::math::Pose2d center{1_m, 2_m, 270_deg};
  constexpr wpi::math::Rectangle2d rect{center, 1_m, 2_m};

  constexpr wpi::math::Translation2d point1{2.5_m, 2_m};
  EXPECT_NEAR(0.5, rect.Distance(point1).value(), kEpsilon);

  constexpr wpi::math::Translation2d point2{1_m, 2_m};
  EXPECT_NEAR(0, rect.Distance(point2).value(), kEpsilon);

  constexpr wpi::math::Translation2d point3{1_m, 1_m};
  EXPECT_NEAR(0.5, rect.Distance(point3).value(), kEpsilon);

  constexpr wpi::math::Translation2d point4{-1_m, 2.5_m};
  EXPECT_NEAR(1, rect.Distance(point4).value(), kEpsilon);
}

TEST(Rectangle2dTest, Nearest) {
  constexpr double kEpsilon = 1E-9;

  constexpr wpi::math::Pose2d center{1_m, 1_m, 90_deg};
  constexpr wpi::math::Rectangle2d rect{center, 3_m, 4_m};

  constexpr wpi::math::Translation2d point1{1_m, 3_m};
  auto nearestPoint1 = rect.Nearest(point1);
  EXPECT_NEAR(1.0, nearestPoint1.X().value(), kEpsilon);
  EXPECT_NEAR(2.5, nearestPoint1.Y().value(), kEpsilon);

  constexpr wpi::math::Translation2d point2{0_m, 0_m};
  auto nearestPoint2 = rect.Nearest(point2);
  EXPECT_NEAR(0.0, nearestPoint2.X().value(), kEpsilon);
  EXPECT_NEAR(0.0, nearestPoint2.Y().value(), kEpsilon);
}

TEST(Rectangle2dTest, Equals) {
  constexpr wpi::math::Pose2d center1{2_m, 3_m, 0_deg};
  constexpr wpi::math::Rectangle2d rect1{center1, 5_m, 3_m};

  constexpr wpi::math::Pose2d center2{2_m, 3_m, 0_deg};
  constexpr wpi::math::Rectangle2d rect2{center2, 5_m, 3_m};

  constexpr wpi::math::Pose2d center3{2_m, 3_m, 0_deg};
  constexpr wpi::math::Rectangle2d rect3{center3, 3_m, 3_m};

  EXPECT_EQ(rect1, rect2);
  EXPECT_NE(rect2, rect3);
}
