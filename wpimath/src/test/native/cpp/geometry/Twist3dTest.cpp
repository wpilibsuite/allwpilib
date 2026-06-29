// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <numbers>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/geometry/Pose3d.hpp"

using namespace wpi::math;

TEST_CASE("Twist3dTest StraightX", "[wpimath]") {
  const Twist3d straight{5_m, 0_m, 0_m, 0_rad, 0_rad, 0_rad};
  const auto straightTransform = straight.Exp();

  Transform3d expected{5_m, 0_m, 0_m, Rotation3d{}};
  CHECK(expected == straightTransform);
}

TEST_CASE("Twist3dTest StraightY", "[wpimath]") {
  const Twist3d straight{0_m, 5_m, 0_m, 0_rad, 0_rad, 0_rad};
  const auto straightTransform = straight.Exp();

  Transform3d expected{0_m, 5_m, 0_m, Rotation3d{}};
  CHECK(expected == straightTransform);
}

TEST_CASE("Twist3dTest StraightZ", "[wpimath]") {
  const Twist3d straight{0_m, 0_m, 5_m, 0_rad, 0_rad, 0_rad};
  const auto straightTransform = straight.Exp();

  Transform3d expected{0_m, 0_m, 5_m, Rotation3d{}};
  CHECK(expected == straightTransform);
}

TEST_CASE("Twist3dTest QuarterCircle", "[wpimath]") {
  Eigen::Vector3d zAxis{0.0, 0.0, 1.0};

  const Twist3d quarterCircle{5_m / 2.0 * std::numbers::pi,
                              0_m,
                              0_m,
                              0_rad,
                              0_rad,
                              wpi::units::radian_t{std::numbers::pi / 2.0}};
  const auto quarterCircleTransform = quarterCircle.Exp();

  Transform3d expected{5_m, 5_m, 0_m, Rotation3d{zAxis, 90_deg}};
  CHECK(expected == quarterCircleTransform);
}

TEST_CASE("Twist3dTest DiagonalNoDtheta", "[wpimath]") {
  const Twist3d diagonal{2_m, 2_m, 0_m, 0_rad, 0_rad, 0_rad};
  const auto diagonalTransform = diagonal.Exp();

  Transform3d expected{2_m, 2_m, 0_m, Rotation3d{}};
  CHECK(expected == diagonalTransform);
}

TEST_CASE("Twist3dTest Equality", "[wpimath]") {
  const Twist3d one{5_m, 1_m, 0_m, 0_rad, 0_rad, 3_rad};
  const Twist3d two{5_m, 1_m, 0_m, 0_rad, 0_rad, 3_rad};
  CHECK(one == two);
}

TEST_CASE("Twist3dTest Inequality", "[wpimath]") {
  const Twist3d one{5_m, 1_m, 0_m, 0_rad, 0_rad, 3_rad};
  const Twist3d two{5_m, 1.2_m, 0_m, 0_rad, 0_rad, 3_rad};
  CHECK(one != two);
}

TEST_CASE("Twist3dTest Pose3dLogX", "[wpimath]") {
  const Pose3d end{0_m, 5_m, 5_m, Rotation3d{90_deg, 0_deg, 0_deg}};
  const Pose3d start;

  const auto twist = (end - start).Log();

  Twist3d expected{0_m,   wpi::units::meter_t{5.0 / 2.0 * std::numbers::pi},
                   0_m,   90_deg,
                   0_deg, 0_deg};
  CHECK(expected == twist);

  // Make sure computed twist gives back original end pose
  const auto reapplied = start + twist.Exp();
  CHECK(end == reapplied);
}

TEST_CASE("Twist3dTest Pose3dLogY", "[wpimath]") {
  const Pose3d end{5_m, 0_m, 5_m, Rotation3d{0_deg, 90_deg, 0_deg}};
  const Pose3d start;

  const auto twist = (end - start).Log();

  Twist3d expected{
      0_m,   0_m,    wpi::units::meter_t{5.0 / 2.0 * std::numbers::pi},
      0_deg, 90_deg, 0_deg};
  CHECK(expected == twist);

  // Make sure computed twist gives back original end pose
  const auto reapplied = start + twist.Exp();
  CHECK(end == reapplied);
}

TEST_CASE("Twist3dTest Pose3dLogZ", "[wpimath]") {
  const Pose3d end{5_m, 5_m, 0_m, Rotation3d{0_deg, 0_deg, 90_deg}};
  const Pose3d start;

  const auto twist = (end - start).Log();

  Twist3d expected{wpi::units::meter_t{5.0 / 2.0 * std::numbers::pi},
                   0_m,
                   0_m,
                   0_deg,
                   0_deg,
                   90_deg};
  CHECK(expected == twist);

  // Make sure computed twist gives back original end pose
  const auto reapplied = start + twist.Exp();
  CHECK(end == reapplied);
}

TEST_CASE("Twist3dTest Constexpr", "[wpimath]") {
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
