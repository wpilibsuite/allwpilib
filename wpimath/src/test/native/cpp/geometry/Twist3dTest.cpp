// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cmath>
#include <numbers>

#include "frc/geometry/Pose3d.h"
#include "gtest/gtest.h"

using namespace frc;

TEST(Twist3dTest, StraightX) {
  const Twist3d straight{5_m, 0_m, 0_m, 0_rad, 0_rad, 0_rad};
  const auto straightPose = Pose3d{}.Exp(straight);

  Pose3d expected{5_m, 0_m, 0_m, Rotation3d{}};
  EXPECT_EQ(expected, straightPose);
}

TEST(Twist3dTest, StraightY) {
  const Twist3d straight{0_m, 5_m, 0_m, 0_rad, 0_rad, 0_rad};
  const auto straightPose = Pose3d{}.Exp(straight);

  Pose3d expected{0_m, 5_m, 0_m, Rotation3d{}};
  EXPECT_EQ(expected, straightPose);
}

TEST(Twist3dTest, StraightZ) {
  const Twist3d straight{0_m, 0_m, 5_m, 0_rad, 0_rad, 0_rad};
  const auto straightPose = Pose3d{}.Exp(straight);

  Pose3d expected{0_m, 0_m, 5_m, Rotation3d{}};
  EXPECT_EQ(expected, straightPose);
}

TEST(Twist3dTest, QuarterCircle) {
  Eigen::Vector3d zAxis{0.0, 0.0, 1.0};

  const Twist3d quarterCircle{
      5_m / 2.0 * std::numbers::pi,           0_m, 0_m, 0_rad, 0_rad,
      units::radian_t{std::numbers::pi / 2.0}};
  const auto quarterCirclePose = Pose3d{}.Exp(quarterCircle);

  Pose3d expected{5_m, 5_m, 0_m, Rotation3d{zAxis, 90_deg}};
  EXPECT_EQ(expected, quarterCirclePose);
}

TEST(Twist3dTest, DiagonalNoDtheta) {
  const Twist3d diagonal{2_m, 2_m, 0_m, 0_rad, 0_rad, 0_rad};
  const auto diagonalPose = Pose3d{}.Exp(diagonal);

  Pose3d expected{2_m, 2_m, 0_m, Rotation3d{}};
  EXPECT_EQ(expected, diagonalPose);
}

TEST(Twist3dTest, Equality) {
  const Twist3d one{5_m, 1_m, 0_m, 0_rad, 0_rad, 3_rad};
  const Twist3d two{5_m, 1_m, 0_m, 0_rad, 0_rad, 3_rad};
  EXPECT_TRUE(one == two);
}

TEST(Twist3dTest, Inequality) {
  const Twist3d one{5_m, 1_m, 0_m, 0_rad, 0_rad, 3_rad};
  const Twist3d two{5_m, 1.2_m, 0_m, 0_rad, 0_rad, 3_rad};
  EXPECT_TRUE(one != two);
}

TEST(Twist3dTest, Pose3dLogX) {
  const Pose3d end{0_m, 5_m, 5_m, Rotation3d{90_deg, 0_deg, 0_deg}};
  const Pose3d start;

  const auto twist = start.Log(end);

  Twist3d expected{0_m,   units::meter_t{5.0 / 2.0 * std::numbers::pi},
                   0_m,   90_deg,
                   0_deg, 0_deg};
  EXPECT_EQ(expected, twist);

  // Make sure computed twist gives back original end pose
  const auto reapplied = start.Exp(twist);
  EXPECT_EQ(end, reapplied);
}

TEST(Twist3dTest, Pose3dLogY) {
  const Pose3d end{5_m, 0_m, 5_m, Rotation3d{0_deg, 90_deg, 0_deg}};
  const Pose3d start;

  const auto twist = start.Log(end);

  Twist3d expected{0_m,   0_m,    units::meter_t{5.0 / 2.0 * std::numbers::pi},
                   0_deg, 90_deg, 0_deg};
  EXPECT_EQ(expected, twist);

  // Make sure computed twist gives back original end pose
  const auto reapplied = start.Exp(twist);
  EXPECT_EQ(end, reapplied);
}

TEST(Twist3dTest, Pose3dLogZ) {
  const Pose3d end{5_m, 5_m, 0_m, Rotation3d{0_deg, 0_deg, 90_deg}};
  const Pose3d start;

  const auto twist = start.Log(end);

  Twist3d expected{units::meter_t{5.0 / 2.0 * std::numbers::pi},
                   0_m,
                   0_m,
                   0_deg,
                   0_deg,
                   90_deg};
  EXPECT_EQ(expected, twist);

  // Make sure computed twist gives back original end pose
  const auto reapplied = start.Exp(twist);
  EXPECT_EQ(end, reapplied);
}

TEST(Twist3dTest, Constexpr) {
  constexpr Twist3d defaultCtor;
  constexpr Twist3d componentCtor{1_m, 2_m, 3_m, 4_rad, 5_rad, 6_rad};
  constexpr auto multiplied = componentCtor * 2;

  static_assert(defaultCtor.dx == 0_m);
  static_assert(componentCtor.dy == 2_m);
  static_assert(componentCtor.dz == 3_m);
  static_assert(multiplied.dx == 2_m);
  static_assert(multiplied.rx == 8_rad);
  static_assert(multiplied.ry == 10_rad);
  static_assert(multiplied.rz == 12_rad);
}
