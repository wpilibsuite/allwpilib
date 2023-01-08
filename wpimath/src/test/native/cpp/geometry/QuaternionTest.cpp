// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <numbers>

#include "frc/geometry/Quaternion.h"
#include "gtest/gtest.h"
#include "units/angle.h"
#include "units/math.h"

using namespace frc;

TEST(QuaternionTest, Init) {
  // Identity
  Quaternion q1;
  EXPECT_DOUBLE_EQ(1.0, q1.W());
  EXPECT_DOUBLE_EQ(0.0, q1.X());
  EXPECT_DOUBLE_EQ(0.0, q1.Y());
  EXPECT_DOUBLE_EQ(0.0, q1.Z());

  // Normalized
  Quaternion q2{0.5, 0.5, 0.5, 0.5};
  EXPECT_DOUBLE_EQ(0.5, q2.W());
  EXPECT_DOUBLE_EQ(0.5, q2.X());
  EXPECT_DOUBLE_EQ(0.5, q2.Y());
  EXPECT_DOUBLE_EQ(0.5, q2.Z());

  // Unnormalized
  Quaternion q3{0.75, 0.3, 0.4, 0.5};
  EXPECT_DOUBLE_EQ(0.75, q3.W());
  EXPECT_DOUBLE_EQ(0.3, q3.X());
  EXPECT_DOUBLE_EQ(0.4, q3.Y());
  EXPECT_DOUBLE_EQ(0.5, q3.Z());

  q3 = q3.Normalize();
  double norm = std::sqrt(0.75 * 0.75 + 0.3 * 0.3 + 0.4 * 0.4 + 0.5 * 0.5);
  EXPECT_DOUBLE_EQ(0.75 / norm, q3.W());
  EXPECT_DOUBLE_EQ(0.3 / norm, q3.X());
  EXPECT_DOUBLE_EQ(0.4 / norm, q3.Y());
  EXPECT_DOUBLE_EQ(0.5 / norm, q3.Z());
  EXPECT_DOUBLE_EQ(1.0, q3.W() * q3.W() + q3.X() * q3.X() + q3.Y() * q3.Y() +
                            q3.Z() * q3.Z());
}

TEST(QuaternionTest, Multiply) {
  // 90° CCW rotations around each axis
  double c = units::math::cos(90_deg / 2.0);
  double s = units::math::sin(90_deg / 2.0);
  Quaternion xRot{c, s, 0.0, 0.0};
  Quaternion yRot{c, 0.0, s, 0.0};
  Quaternion zRot{c, 0.0, 0.0, s};

  // 90° CCW X rotation, 90° CCW Y rotation, and 90° CCW Z rotation should
  // produce a 90° CCW Y rotation
  auto expected = yRot;
  auto actual = zRot * yRot * xRot;
  EXPECT_NEAR(expected.W(), actual.W(), 1e-9);
  EXPECT_NEAR(expected.X(), actual.X(), 1e-9);
  EXPECT_NEAR(expected.Y(), actual.Y(), 1e-9);
  EXPECT_NEAR(expected.Z(), actual.Z(), 1e-9);

  // Identity
  Quaternion q{0.72760687510899891, 0.29104275004359953, 0.38805700005813276,
               0.48507125007266594};
  actual = q * q.Inverse();
  EXPECT_DOUBLE_EQ(1.0, actual.W());
  EXPECT_DOUBLE_EQ(0.0, actual.X());
  EXPECT_DOUBLE_EQ(0.0, actual.Y());
  EXPECT_DOUBLE_EQ(0.0, actual.Z());
}

TEST(QuaternionTest, Inverse) {
  Quaternion q{0.72760687510899891, 0.29104275004359953, 0.38805700005813276,
               0.48507125007266594};
  auto inv = q.Inverse();

  EXPECT_DOUBLE_EQ(q.W(), inv.W());
  EXPECT_DOUBLE_EQ(-q.X(), inv.X());
  EXPECT_DOUBLE_EQ(-q.Y(), inv.Y());
  EXPECT_DOUBLE_EQ(-q.Z(), inv.Z());
}
