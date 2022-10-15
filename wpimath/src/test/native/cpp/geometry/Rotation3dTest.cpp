// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cmath>
#include <numbers>

#include <wpi/MathExtras.h>

#include "frc/EigenCore.h"
#include "frc/geometry/Rotation3d.h"
#include "gtest/gtest.h"

using namespace frc;

TEST(Rotation3dTest, InitAxisAngleAndRollPitchYaw) {
  const Eigen::Vector3d xAxis{1.0, 0.0, 0.0};
  const Rotation3d rot1{xAxis, units::radian_t{std::numbers::pi / 3}};
  const Rotation3d rot2{units::radian_t{std::numbers::pi / 3}, 0_rad, 0_rad};
  EXPECT_EQ(rot1, rot2);

  const Eigen::Vector3d yAxis{0.0, 1.0, 0.0};
  const Rotation3d rot3{yAxis, units::radian_t{std::numbers::pi / 3}};
  const Rotation3d rot4{0_rad, units::radian_t{std::numbers::pi / 3}, 0_rad};
  EXPECT_EQ(rot3, rot4);

  const Eigen::Vector3d zAxis{0.0, 0.0, 1.0};
  const Rotation3d rot5{zAxis, units::radian_t{std::numbers::pi / 3}};
  const Rotation3d rot6{0_rad, 0_rad, units::radian_t{std::numbers::pi / 3}};
  EXPECT_EQ(rot5, rot6);
}

TEST(Rotation3dTest, InitRotationMatrix) {
  // No rotation
  const Matrixd<3, 3> R1 = Matrixd<3, 3>::Identity();
  const Rotation3d rot1{R1};
  EXPECT_EQ(Rotation3d{}, rot1);

  // 90 degree CCW rotation around z-axis
  Matrixd<3, 3> R2;
  R2.block<3, 1>(0, 0) = Vectord<3>{0.0, 1.0, 0.0};
  R2.block<3, 1>(0, 1) = Vectord<3>{-1.0, 0.0, 0.0};
  R2.block<3, 1>(0, 2) = Vectord<3>{0.0, 0.0, 1.0};
  const Rotation3d rot2{R2};
  const Rotation3d expected2{0_deg, 0_deg, 90_deg};
  EXPECT_EQ(expected2, rot2);

  // Matrix that isn't orthogonal
  const Matrixd<3, 3> R3{{1.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {1.0, 0.0, 0.0}};
  EXPECT_THROW(Rotation3d{R3}, std::domain_error);

  // Matrix that's orthogonal but not special orthogonal
  const Matrixd<3, 3> R4 = Matrixd<3, 3>::Identity() * 2.0;
  EXPECT_THROW(Rotation3d{R4}, std::domain_error);
}

TEST(Rotation3dTest, InitTwoVector) {
  const Eigen::Vector3d xAxis{1.0, 0.0, 0.0};
  const Eigen::Vector3d yAxis{0.0, 1.0, 0.0};
  const Eigen::Vector3d zAxis{0.0, 0.0, 1.0};

  // 90 degree CW rotation around y-axis
  const Rotation3d rot1{xAxis, zAxis};
  const Rotation3d expected1{yAxis, units::radian_t{-std::numbers::pi / 2.0}};
  EXPECT_EQ(expected1, rot1);

  // 45 degree CCW rotation around z-axis
  const Rotation3d rot2{xAxis, Eigen::Vector3d{1.0, 1.0, 0.0}};
  const Rotation3d expected2{zAxis, units::radian_t{std::numbers::pi / 4.0}};
  EXPECT_EQ(expected2, rot2);

  // 0 degree rotation of x-axes
  const Rotation3d rot3{xAxis, xAxis};
  EXPECT_EQ(Rotation3d{}, rot3);

  // 0 degree rotation of y-axes
  const Rotation3d rot4{yAxis, yAxis};
  EXPECT_EQ(Rotation3d{}, rot4);

  // 0 degree rotation of z-axes
  const Rotation3d rot5{zAxis, zAxis};
  EXPECT_EQ(Rotation3d{}, rot5);

  // 180 degree rotation tests. For 180 degree rotations, any quaternion with an
  // orthogonal rotation axis is acceptable. The rotation axis and initial
  // vector are orthogonal if their dot product is zero.

  // 180 degree rotation of x-axes
  const Rotation3d rot6{xAxis, -xAxis};
  const auto q6 = rot6.GetQuaternion();
  EXPECT_EQ(0.0, q6.W());
  EXPECT_EQ(0.0, q6.X() * xAxis(0) + q6.Y() * xAxis(1) + q6.Z() * xAxis(2));

  // 180 degree rotation of y-axes
  const Rotation3d rot7{yAxis, -yAxis};
  const auto q7 = rot7.GetQuaternion();
  EXPECT_EQ(0.0, q7.W());
  EXPECT_EQ(0.0, q7.X() * yAxis(0) + q7.Y() * yAxis(1) + q7.Z() * yAxis(2));

  // 180 degree rotation of z-axes
  const Rotation3d rot8{zAxis, -zAxis};
  const auto q8 = rot8.GetQuaternion();
  EXPECT_EQ(0.0, q8.W());
  EXPECT_EQ(0.0, q8.X() * zAxis(0) + q8.Y() * zAxis(1) + q8.Z() * zAxis(2));
}

TEST(Rotation3dTest, RadiansToDegrees) {
  const Eigen::Vector3d zAxis{0.0, 0.0, 1.0};

  const Rotation3d rot1{zAxis, units::radian_t{std::numbers::pi / 3}};
  EXPECT_DOUBLE_EQ(0.0, rot1.X().value());
  EXPECT_DOUBLE_EQ(0.0, rot1.Y().value());
  EXPECT_DOUBLE_EQ(units::radian_t{60_deg}.value(), rot1.Z().value());

  const Rotation3d rot2{zAxis, units::radian_t{std::numbers::pi / 4}};
  EXPECT_DOUBLE_EQ(0.0, rot2.X().value());
  EXPECT_DOUBLE_EQ(0.0, rot2.Y().value());
  EXPECT_DOUBLE_EQ(units::radian_t{45_deg}.value(), rot2.Z().value());
}

TEST(Rotation3dTest, DegreesToRadians) {
  const Eigen::Vector3d zAxis{0.0, 0.0, 1.0};

  const auto rot1 = Rotation3d{zAxis, 45_deg};
  EXPECT_DOUBLE_EQ(0.0, rot1.X().value());
  EXPECT_DOUBLE_EQ(0.0, rot1.Y().value());
  EXPECT_DOUBLE_EQ(std::numbers::pi / 4.0, rot1.Z().value());

  const auto rot2 = Rotation3d{zAxis, 30_deg};
  EXPECT_DOUBLE_EQ(0.0, rot2.X().value());
  EXPECT_DOUBLE_EQ(0.0, rot2.Y().value());
  EXPECT_DOUBLE_EQ(std::numbers::pi / 6.0, rot2.Z().value());
}

TEST(Rotation3dTest, RotationLoop) {
  Rotation3d rot;

  rot = rot + Rotation3d{90_deg, 0_deg, 0_deg};
  Rotation3d expected{90_deg, 0_deg, 0_deg};
  EXPECT_EQ(expected, rot);

  rot = rot + Rotation3d{0_deg, 90_deg, 0_deg};
  expected = Rotation3d{
      {1.0 / std::sqrt(3), 1.0 / std::sqrt(3), -1.0 / std::sqrt(3)}, 120_deg};
  EXPECT_EQ(expected, rot);

  rot = rot + Rotation3d{0_deg, 0_deg, 90_deg};
  expected = Rotation3d{0_deg, 90_deg, 0_deg};
  EXPECT_EQ(expected, rot);

  rot = rot + Rotation3d{0_deg, -90_deg, 0_deg};
  EXPECT_EQ(Rotation3d{}, rot);
}

TEST(Rotation3dTest, RotateByFromZeroX) {
  const Eigen::Vector3d xAxis{1.0, 0.0, 0.0};

  const Rotation3d zero;
  auto rotated = zero + Rotation3d{xAxis, 90_deg};

  Rotation3d expected{xAxis, 90_deg};
  EXPECT_EQ(expected, rotated);
}

TEST(Rotation3dTest, RotateByFromZeroY) {
  const Eigen::Vector3d yAxis{0.0, 1.0, 0.0};

  const Rotation3d zero;
  auto rotated = zero + Rotation3d{yAxis, 90_deg};

  Rotation3d expected{yAxis, 90_deg};
  EXPECT_EQ(expected, rotated);
}

TEST(Rotation3dTest, RotateByFromZeroZ) {
  const Eigen::Vector3d zAxis{0.0, 0.0, 1.0};

  const Rotation3d zero;
  auto rotated = zero + Rotation3d{zAxis, 90_deg};

  Rotation3d expected{zAxis, 90_deg};
  EXPECT_EQ(expected, rotated);
}

TEST(Rotation3dTest, RotateByNonZeroX) {
  const Eigen::Vector3d xAxis{1.0, 0.0, 0.0};

  auto rot = Rotation3d{xAxis, 90_deg};
  rot = rot + Rotation3d{xAxis, 30_deg};

  Rotation3d expected{xAxis, 120_deg};
  EXPECT_EQ(expected, rot);
}

TEST(Rotation3dTest, RotateByNonZeroY) {
  const Eigen::Vector3d yAxis{0.0, 1.0, 0.0};

  auto rot = Rotation3d{yAxis, 90_deg};
  rot = rot + Rotation3d{yAxis, 30_deg};

  Rotation3d expected{yAxis, 120_deg};
  EXPECT_EQ(expected, rot);
}

TEST(Rotation3dTest, RotateByNonZeroZ) {
  const Eigen::Vector3d zAxis{0.0, 0.0, 1.0};

  auto rot = Rotation3d{zAxis, 90_deg};
  rot = rot + Rotation3d{zAxis, 30_deg};

  Rotation3d expected{zAxis, 120_deg};
  EXPECT_EQ(expected, rot);
}

TEST(Rotation3dTest, Minus) {
  const Eigen::Vector3d zAxis{0.0, 0.0, 1.0};

  const auto rot1 = Rotation3d{zAxis, 70_deg};
  const auto rot2 = Rotation3d{zAxis, 30_deg};

  EXPECT_DOUBLE_EQ(40.0, units::degree_t{(rot1 - rot2).Z()}.value());
}

TEST(Rotation3dTest, AxisAngle) {
  const Eigen::Vector3d xAxis{1.0, 0.0, 0.0};
  const Eigen::Vector3d yAxis{0.0, 1.0, 0.0};
  const Eigen::Vector3d zAxis{0.0, 0.0, 1.0};

  Rotation3d rot1{xAxis, 90_deg};
  EXPECT_EQ(xAxis, rot1.Axis());
  EXPECT_DOUBLE_EQ(std::numbers::pi / 2.0, rot1.Angle().value());

  Rotation3d rot2{yAxis, 45_deg};
  EXPECT_EQ(yAxis, rot2.Axis());
  EXPECT_DOUBLE_EQ(std::numbers::pi / 4.0, rot2.Angle().value());

  Rotation3d rot3{zAxis, 60_deg};
  EXPECT_EQ(zAxis, rot3.Axis());
  EXPECT_DOUBLE_EQ(std::numbers::pi / 3.0, rot3.Angle().value());
}

TEST(Rotation3dTest, ToRotation2d) {
  Rotation3d rotation{20_deg, 30_deg, 40_deg};
  Rotation2d expected{40_deg};

  EXPECT_EQ(expected, rotation.ToRotation2d());
}

TEST(Rotation3dTest, Equality) {
  const Eigen::Vector3d zAxis{0.0, 0.0, 1.0};

  const auto rot1 = Rotation3d{zAxis, 43_deg};
  const auto rot2 = Rotation3d{zAxis, 43_deg};
  EXPECT_EQ(rot1, rot2);

  const auto rot3 = Rotation3d{zAxis, -180_deg};
  const auto rot4 = Rotation3d{zAxis, 180_deg};
  EXPECT_EQ(rot3, rot4);
}

TEST(Rotation3dTest, Inequality) {
  const Eigen::Vector3d zAxis{0.0, 0.0, 1.0};

  const auto rot1 = Rotation3d{zAxis, 43_deg};
  const auto rot2 = Rotation3d{zAxis, 43.5_deg};
  EXPECT_NE(rot1, rot2);
}

TEST(Rotation3dTest, Interpolate) {
  const Eigen::Vector3d xAxis{1.0, 0.0, 0.0};
  const Eigen::Vector3d yAxis{0.0, 1.0, 0.0};
  const Eigen::Vector3d zAxis{0.0, 0.0, 1.0};

  // 50 + (70 - 50) * 0.5 = 60
  auto rot1 = Rotation3d{xAxis, 50_deg};
  auto rot2 = Rotation3d{xAxis, 70_deg};
  auto interpolated = wpi::Lerp(rot1, rot2, 0.5);
  EXPECT_DOUBLE_EQ(60.0, units::degree_t{interpolated.X()}.value());
  EXPECT_DOUBLE_EQ(0.0, units::degree_t{interpolated.Y()}.value());
  EXPECT_DOUBLE_EQ(0.0, units::degree_t{interpolated.Z()}.value());

  // -160 minus half distance between 170 and -160 (15) = -175
  rot1 = Rotation3d{xAxis, 170_deg};
  rot2 = Rotation3d{xAxis, -160_deg};
  interpolated = wpi::Lerp(rot1, rot2, 0.5);
  EXPECT_DOUBLE_EQ(-175.0, units::degree_t{interpolated.X()}.value());
  EXPECT_DOUBLE_EQ(0.0, units::degree_t{interpolated.Y()}.value());
  EXPECT_DOUBLE_EQ(0.0, units::degree_t{interpolated.Z()}.value());

  // 50 + (70 - 50) * 0.5 = 60
  rot1 = Rotation3d{yAxis, 50_deg};
  rot2 = Rotation3d{yAxis, 70_deg};
  interpolated = wpi::Lerp(rot1, rot2, 0.5);
  EXPECT_DOUBLE_EQ(0.0, units::degree_t{interpolated.X()}.value());
  EXPECT_DOUBLE_EQ(60.0, units::degree_t{interpolated.Y()}.value());
  EXPECT_DOUBLE_EQ(0.0, units::degree_t{interpolated.Z()}.value());

  // -160 plus half distance between 170 and -160 (165) = 5
  rot1 = Rotation3d{yAxis, 170_deg};
  rot2 = Rotation3d{yAxis, -160_deg};
  interpolated = wpi::Lerp(rot1, rot2, 0.5);
  EXPECT_DOUBLE_EQ(180.0, units::degree_t{interpolated.X()}.value());
  EXPECT_DOUBLE_EQ(-5.0, units::degree_t{interpolated.Y()}.value());
  EXPECT_DOUBLE_EQ(180.0, units::degree_t{interpolated.Z()}.value());

  // 50 + (70 - 50) * 0.5 = 60
  rot1 = Rotation3d{zAxis, 50_deg};
  rot2 = Rotation3d{zAxis, 70_deg};
  interpolated = wpi::Lerp(rot1, rot2, 0.5);
  EXPECT_DOUBLE_EQ(0.0, units::degree_t{interpolated.X()}.value());
  EXPECT_DOUBLE_EQ(0.0, units::degree_t{interpolated.Y()}.value());
  EXPECT_DOUBLE_EQ(60.0, units::degree_t{interpolated.Z()}.value());

  // -160 minus half distance between 170 and -160 (15) = -175
  rot1 = Rotation3d{zAxis, 170_deg};
  rot2 = Rotation3d{zAxis, -160_deg};
  interpolated = wpi::Lerp(rot1, rot2, 0.5);
  EXPECT_DOUBLE_EQ(0.0, units::degree_t{interpolated.X()}.value());
  EXPECT_DOUBLE_EQ(0.0, units::degree_t{interpolated.Y()}.value());
  EXPECT_DOUBLE_EQ(-175.0, units::degree_t{interpolated.Z()}.value());
}
