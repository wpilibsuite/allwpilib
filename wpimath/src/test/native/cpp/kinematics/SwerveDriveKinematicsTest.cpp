// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/SwerveDriveKinematics.hpp"

#include <numbers>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/geometry/Translation2d.hpp"
#include "wpi/units/angular_velocity.hpp"

using namespace wpi::math;

static constexpr double kEpsilon = 0.1;

class SwerveDriveKinematicsTest {
 protected:
  Translation2d m_fl{12_m, 12_m};
  Translation2d m_fr{12_m, -12_m};
  Translation2d m_bl{-12_m, 12_m};
  Translation2d m_br{-12_m, -12_m};

  SwerveDriveKinematics<4> m_kinematics{m_fl, m_fr, m_bl, m_br};
};

TEST_CASE_METHOD(SwerveDriveKinematicsTest,
                 "SwerveDriveKinematicsTest StraightLineInverseKinematics",
                 "[wpimath]") {
  ChassisVelocities velocities{5.0_mps, 0.0_mps, 0.0_rad_per_s};

  auto [fl, fr, bl, br] = m_kinematics.ToSwerveModuleVelocities(velocities);

  CHECK(fl.velocity.value() == Catch::Approx(5.0).margin(kEpsilon));
  CHECK(fr.velocity.value() == Catch::Approx(5.0).margin(kEpsilon));
  CHECK(bl.velocity.value() == Catch::Approx(5.0).margin(kEpsilon));
  CHECK(br.velocity.value() == Catch::Approx(5.0).margin(kEpsilon));

  CHECK(fl.angle.Radians().value() == Catch::Approx(0.0).margin(kEpsilon));
  CHECK(fr.angle.Radians().value() == Catch::Approx(0.0).margin(kEpsilon));
  CHECK(bl.angle.Radians().value() == Catch::Approx(0.0).margin(kEpsilon));
  CHECK(br.angle.Radians().value() == Catch::Approx(0.0).margin(kEpsilon));
}

TEST_CASE_METHOD(SwerveDriveKinematicsTest,
                 "SwerveDriveKinematicsTest StraightLineForwardKinematics",
                 "[wpimath]") {
  SwerveModuleVelocity state{5.0_mps, 0_deg};

  auto chassisVelocities =
      m_kinematics.ToChassisVelocities(state, state, state, state);

  CHECK(chassisVelocities.vx.value() == Catch::Approx(5.0).margin(kEpsilon));
  CHECK(chassisVelocities.vy.value() == Catch::Approx(0.0).margin(kEpsilon));
  CHECK(chassisVelocities.omega.value() == Catch::Approx(0.0).margin(kEpsilon));
}

TEST_CASE_METHOD(
    SwerveDriveKinematicsTest,
    "SwerveDriveKinematicsTest StraightLineForwardKinematicsWithDeltas",
    "[wpimath]") {
  SwerveModulePosition delta{5.0_m, 0_deg};

  auto twist = m_kinematics.ToTwist2d(delta, delta, delta, delta);

  CHECK(twist.dx.value() == Catch::Approx(5.0).margin(kEpsilon));
  CHECK(twist.dy.value() == Catch::Approx(0.0).margin(kEpsilon));
  CHECK(twist.dtheta.value() == Catch::Approx(0.0).margin(kEpsilon));
}

TEST_CASE_METHOD(SwerveDriveKinematicsTest,
                 "SwerveDriveKinematicsTest StraightStrafeInverseKinematics",
                 "[wpimath]") {
  ChassisVelocities velocities{0_mps, 5_mps, 0_rad_per_s};
  auto [fl, fr, bl, br] = m_kinematics.ToSwerveModuleVelocities(velocities);

  CHECK(fl.velocity.value() == Catch::Approx(5.0).margin(kEpsilon));
  CHECK(fr.velocity.value() == Catch::Approx(5.0).margin(kEpsilon));
  CHECK(bl.velocity.value() == Catch::Approx(5.0).margin(kEpsilon));
  CHECK(br.velocity.value() == Catch::Approx(5.0).margin(kEpsilon));

  CHECK(fl.angle.Degrees().value() == Catch::Approx(90.0).margin(kEpsilon));
  CHECK(fr.angle.Degrees().value() == Catch::Approx(90.0).margin(kEpsilon));
  CHECK(bl.angle.Degrees().value() == Catch::Approx(90.0).margin(kEpsilon));
  CHECK(br.angle.Degrees().value() == Catch::Approx(90.0).margin(kEpsilon));
}

TEST_CASE_METHOD(SwerveDriveKinematicsTest,
                 "SwerveDriveKinematicsTest StraightStrafeForwardKinematics",
                 "[wpimath]") {
  SwerveModuleVelocity state{5_mps, 90_deg};
  auto chassisVelocities =
      m_kinematics.ToChassisVelocities(state, state, state, state);

  CHECK(chassisVelocities.vx.value() == Catch::Approx(0.0).margin(kEpsilon));
  CHECK(chassisVelocities.vy.value() == Catch::Approx(5.0).margin(kEpsilon));
  CHECK(chassisVelocities.omega.value() == Catch::Approx(0.0).margin(kEpsilon));
}

TEST_CASE_METHOD(
    SwerveDriveKinematicsTest,
    "SwerveDriveKinematicsTest StraightStrafeForwardKinematicsWithDeltas",
    "[wpimath]") {
  SwerveModulePosition delta{5_m, 90_deg};

  auto twist = m_kinematics.ToTwist2d(delta, delta, delta, delta);

  CHECK(twist.dx.value() == Catch::Approx(0.0).margin(kEpsilon));
  CHECK(twist.dy.value() == Catch::Approx(5.0).margin(kEpsilon));
  CHECK(twist.dtheta.value() == Catch::Approx(0.0).margin(kEpsilon));
}

TEST_CASE_METHOD(SwerveDriveKinematicsTest,
                 "SwerveDriveKinematicsTest TurnInPlaceInverseKinematics",
                 "[wpimath]") {
  ChassisVelocities velocities{
      0_mps, 0_mps, wpi::units::radians_per_second_t{2 * std::numbers::pi}};
  auto [fl, fr, bl, br] = m_kinematics.ToSwerveModuleVelocities(velocities);

  CHECK(fl.velocity.value() == Catch::Approx(106.63).margin(kEpsilon));
  CHECK(fr.velocity.value() == Catch::Approx(106.63).margin(kEpsilon));
  CHECK(bl.velocity.value() == Catch::Approx(106.63).margin(kEpsilon));
  CHECK(br.velocity.value() == Catch::Approx(106.63).margin(kEpsilon));

  CHECK(fl.angle.Degrees().value() == Catch::Approx(135.0).margin(kEpsilon));
  CHECK(fr.angle.Degrees().value() == Catch::Approx(45.0).margin(kEpsilon));
  CHECK(bl.angle.Degrees().value() == Catch::Approx(-135.0).margin(kEpsilon));
  CHECK(br.angle.Degrees().value() == Catch::Approx(-45.0).margin(kEpsilon));
}

TEST_CASE_METHOD(SwerveDriveKinematicsTest,
                 "SwerveDriveKinematicsTest ConserveWheelAngle", "[wpimath]") {
  ChassisVelocities velocities{
      0_mps, 0_mps, wpi::units::radians_per_second_t{2 * std::numbers::pi}};
  m_kinematics.ToSwerveModuleVelocities(velocities);
  auto [fl, fr, bl, br] =
      m_kinematics.ToSwerveModuleVelocities(ChassisVelocities{});

  CHECK(fl.velocity.value() == Catch::Approx(0.0).margin(kEpsilon));
  CHECK(fr.velocity.value() == Catch::Approx(0.0).margin(kEpsilon));
  CHECK(bl.velocity.value() == Catch::Approx(0.0).margin(kEpsilon));
  CHECK(br.velocity.value() == Catch::Approx(0.0).margin(kEpsilon));

  CHECK(fl.angle.Degrees().value() == Catch::Approx(135.0).margin(kEpsilon));
  CHECK(fr.angle.Degrees().value() == Catch::Approx(45.0).margin(kEpsilon));
  CHECK(bl.angle.Degrees().value() == Catch::Approx(-135.0).margin(kEpsilon));
  CHECK(br.angle.Degrees().value() == Catch::Approx(-45.0).margin(kEpsilon));
}
TEST_CASE_METHOD(SwerveDriveKinematicsTest,
                 "SwerveDriveKinematicsTest ResetWheelAngle", "[wpimath]") {
  Rotation2d fl = {0_deg};
  Rotation2d fr = {90_deg};
  Rotation2d bl = {180_deg};
  Rotation2d br = {270_deg};
  m_kinematics.ResetHeadings(fl, fr, bl, br);
  auto [flMod, frMod, blMod, brMod] =
      m_kinematics.ToSwerveModuleVelocities(ChassisVelocities{});

  CHECK(flMod.velocity.value() == Catch::Approx(0.0).margin(kEpsilon));
  CHECK(frMod.velocity.value() == Catch::Approx(0.0).margin(kEpsilon));
  CHECK(blMod.velocity.value() == Catch::Approx(0.0).margin(kEpsilon));
  CHECK(brMod.velocity.value() == Catch::Approx(0.0).margin(kEpsilon));

  CHECK(flMod.angle.Degrees().value() == Catch::Approx(0.0).margin(kEpsilon));
  CHECK(frMod.angle.Degrees().value() == Catch::Approx(90.0).margin(kEpsilon));
  CHECK(blMod.angle.Degrees().value() == Catch::Approx(180.0).margin(kEpsilon));
  CHECK(brMod.angle.Degrees().value() == Catch::Approx(-90.0).margin(kEpsilon));
}

TEST_CASE_METHOD(SwerveDriveKinematicsTest,
                 "SwerveDriveKinematicsTest TurnInPlaceForwardKinematics",
                 "[wpimath]") {
  SwerveModuleVelocity fl{106.629_mps, 135_deg};
  SwerveModuleVelocity fr{106.629_mps, 45_deg};
  SwerveModuleVelocity bl{106.629_mps, -135_deg};
  SwerveModuleVelocity br{106.629_mps, -45_deg};

  auto chassisVelocities = m_kinematics.ToChassisVelocities(fl, fr, bl, br);

  CHECK(chassisVelocities.vx.value() == Catch::Approx(0.0).margin(kEpsilon));
  CHECK(chassisVelocities.vy.value() == Catch::Approx(0.0).margin(kEpsilon));
  CHECK(chassisVelocities.omega.value() ==
        Catch::Approx(2 * std::numbers::pi).margin(kEpsilon));
}

TEST_CASE_METHOD(
    SwerveDriveKinematicsTest,
    "SwerveDriveKinematicsTest TurnInPlaceForwardKinematicsWithDeltas",
    "[wpimath]") {
  SwerveModulePosition fl{106.629_m, 135_deg};
  SwerveModulePosition fr{106.629_m, 45_deg};
  SwerveModulePosition bl{106.629_m, -135_deg};
  SwerveModulePosition br{106.629_m, -45_deg};

  auto twist = m_kinematics.ToTwist2d(fl, fr, bl, br);

  CHECK(twist.dx.value() == Catch::Approx(0.0).margin(kEpsilon));
  CHECK(twist.dy.value() == Catch::Approx(0.0).margin(kEpsilon));
  CHECK(twist.dtheta.value() ==
        Catch::Approx(2 * std::numbers::pi).margin(kEpsilon));
}

TEST_CASE_METHOD(
    SwerveDriveKinematicsTest,
    "SwerveDriveKinematicsTest OffCenterCORRotationInverseKinematics",
    "[wpimath]") {
  ChassisVelocities velocities{
      0_mps, 0_mps, wpi::units::radians_per_second_t{2 * std::numbers::pi}};
  auto [fl, fr, bl, br] =
      m_kinematics.ToSwerveModuleVelocities(velocities, m_fl);

  CHECK(fl.velocity.value() == Catch::Approx(0.0).margin(kEpsilon));
  CHECK(fr.velocity.value() == Catch::Approx(150.796).margin(kEpsilon));
  CHECK(bl.velocity.value() == Catch::Approx(150.796).margin(kEpsilon));
  CHECK(br.velocity.value() == Catch::Approx(213.258).margin(kEpsilon));

  CHECK(fl.angle.Degrees().value() == Catch::Approx(0.0).margin(kEpsilon));
  CHECK(fr.angle.Degrees().value() == Catch::Approx(0.0).margin(kEpsilon));
  CHECK(bl.angle.Degrees().value() == Catch::Approx(-90.0).margin(kEpsilon));
  CHECK(br.angle.Degrees().value() == Catch::Approx(-45.0).margin(kEpsilon));
}

TEST_CASE_METHOD(
    SwerveDriveKinematicsTest,
    "SwerveDriveKinematicsTest OffCenterCORRotationForwardKinematics",
    "[wpimath]") {
  SwerveModuleVelocity fl{0.0_mps, 0_deg};
  SwerveModuleVelocity fr{150.796_mps, 0_deg};
  SwerveModuleVelocity bl{150.796_mps, -90_deg};
  SwerveModuleVelocity br{213.258_mps, -45_deg};

  auto chassisVelocities = m_kinematics.ToChassisVelocities(fl, fr, bl, br);

  CHECK(chassisVelocities.vx.value() == Catch::Approx(75.398).margin(kEpsilon));
  CHECK(chassisVelocities.vy.value() ==
        Catch::Approx(-75.398).margin(kEpsilon));
  CHECK(chassisVelocities.omega.value() ==
        Catch::Approx(2 * std::numbers::pi).margin(kEpsilon));
}

TEST_CASE_METHOD(
    SwerveDriveKinematicsTest,
    "SwerveDriveKinematicsTest OffCenterCORRotationForwardKinematicsWithDeltas",
    "[wpimath]") {
  SwerveModulePosition fl{0.0_m, 0_deg};
  SwerveModulePosition fr{150.796_m, 0_deg};
  SwerveModulePosition bl{150.796_m, -90_deg};
  SwerveModulePosition br{213.258_m, -45_deg};

  auto twist = m_kinematics.ToTwist2d(fl, fr, bl, br);

  CHECK(twist.dx.value() == Catch::Approx(75.398).margin(kEpsilon));
  CHECK(twist.dy.value() == Catch::Approx(-75.398).margin(kEpsilon));
  CHECK(twist.dtheta.value() ==
        Catch::Approx(2 * std::numbers::pi).margin(kEpsilon));
}

TEST_CASE_METHOD(SwerveDriveKinematicsTest,
                 "SwerveDriveKinematicsTest "
                 "OffCenterCORRotationAndTranslationInverseKinematics",
                 "[wpimath]") {
  ChassisVelocities velocities{0_mps, 3.0_mps, 1.5_rad_per_s};
  auto [fl, fr, bl, br] = m_kinematics.ToSwerveModuleVelocities(
      velocities, Translation2d{24_m, 0_m});

  CHECK(fl.velocity.value() == Catch::Approx(23.43).margin(kEpsilon));
  CHECK(fr.velocity.value() == Catch::Approx(23.43).margin(kEpsilon));
  CHECK(bl.velocity.value() == Catch::Approx(54.08).margin(kEpsilon));
  CHECK(br.velocity.value() == Catch::Approx(54.08).margin(kEpsilon));

  CHECK(fl.angle.Degrees().value() == Catch::Approx(-140.19).margin(kEpsilon));
  CHECK(fr.angle.Degrees().value() == Catch::Approx(-39.81).margin(kEpsilon));
  CHECK(bl.angle.Degrees().value() == Catch::Approx(-109.44).margin(kEpsilon));
  CHECK(br.angle.Degrees().value() == Catch::Approx(-70.56).margin(kEpsilon));
}

TEST_CASE_METHOD(SwerveDriveKinematicsTest,
                 "SwerveDriveKinematicsTest "
                 "OffCenterCORRotationAndTranslationForwardKinematics",
                 "[wpimath]") {
  SwerveModuleVelocity fl{23.43_mps, -140.19_deg};
  SwerveModuleVelocity fr{23.43_mps, -39.81_deg};
  SwerveModuleVelocity bl{54.08_mps, -109.44_deg};
  SwerveModuleVelocity br{54.08_mps, -70.56_deg};

  auto chassisVelocities = m_kinematics.ToChassisVelocities(fl, fr, bl, br);

  CHECK(chassisVelocities.vx.value() == Catch::Approx(0.0).margin(kEpsilon));
  CHECK(chassisVelocities.vy.value() == Catch::Approx(-33.0).margin(kEpsilon));
  CHECK(chassisVelocities.omega.value() == Catch::Approx(1.5).margin(kEpsilon));
}

TEST_CASE_METHOD(
    SwerveDriveKinematicsTest,
    "SwerveDriveKinematicsTest "
    "OffCenterCORRotationAndTranslationForwardKinematicsWithDeltas",
    "[wpimath]") {
  SwerveModulePosition fl{23.43_m, -140.19_deg};
  SwerveModulePosition fr{23.43_m, -39.81_deg};
  SwerveModulePosition bl{54.08_m, -109.44_deg};
  SwerveModulePosition br{54.08_m, -70.56_deg};

  auto twist = m_kinematics.ToTwist2d(fl, fr, bl, br);

  CHECK(twist.dx.value() == Catch::Approx(0.0).margin(kEpsilon));
  CHECK(twist.dy.value() == Catch::Approx(-33.0).margin(kEpsilon));
  CHECK(twist.dtheta.value() == Catch::Approx(1.5).margin(kEpsilon));
}

TEST_CASE_METHOD(SwerveDriveKinematicsTest,
                 "SwerveDriveKinematicsTest Desaturate", "[wpimath]") {
  SwerveModuleVelocity state1{5.0_mps, 0_deg};
  SwerveModuleVelocity state2{6.0_mps, 0_deg};
  SwerveModuleVelocity state3{4.0_mps, 0_deg};
  SwerveModuleVelocity state4{7.0_mps, 0_deg};

  wpi::util::array<SwerveModuleVelocity, 4> arr{state1, state2, state3, state4};
  arr = SwerveDriveKinematics<4>::DesaturateWheelVelocities(arr, 5.5_mps);

  double kFactor = 5.5 / 7.0;

  CHECK(arr[0].velocity.value() ==
        Catch::Approx(5.0 * kFactor).margin(kEpsilon));
  CHECK(arr[1].velocity.value() ==
        Catch::Approx(6.0 * kFactor).margin(kEpsilon));
  CHECK(arr[2].velocity.value() ==
        Catch::Approx(4.0 * kFactor).margin(kEpsilon));
  CHECK(arr[3].velocity.value() ==
        Catch::Approx(7.0 * kFactor).margin(kEpsilon));
}

TEST_CASE_METHOD(SwerveDriveKinematicsTest,
                 "SwerveDriveKinematicsTest DesaturateSmooth", "[wpimath]") {
  SwerveModuleVelocity state1{5.0_mps, 0_deg};
  SwerveModuleVelocity state2{6.0_mps, 0_deg};
  SwerveModuleVelocity state3{4.0_mps, 0_deg};
  SwerveModuleVelocity state4{7.0_mps, 0_deg};

  wpi::util::array<SwerveModuleVelocity, 4> arr{state1, state2, state3, state4};
  arr = SwerveDriveKinematics<4>::DesaturateWheelVelocities(
      arr, m_kinematics.ToChassisVelocities(arr), 5.5_mps, 5.5_mps,
      3.5_rad_per_s);

  double kFactor = 5.5 / 7.0;

  CHECK(arr[0].velocity.value() ==
        Catch::Approx(5.0 * kFactor).margin(kEpsilon));
  CHECK(arr[1].velocity.value() ==
        Catch::Approx(6.0 * kFactor).margin(kEpsilon));
  CHECK(arr[2].velocity.value() ==
        Catch::Approx(4.0 * kFactor).margin(kEpsilon));
  CHECK(arr[3].velocity.value() ==
        Catch::Approx(7.0 * kFactor).margin(kEpsilon));
}

TEST_CASE_METHOD(SwerveDriveKinematicsTest,
                 "SwerveDriveKinematicsTest DesaturateNegativeVelocity",
                 "[wpimath]") {
  SwerveModuleVelocity state1{1.0_mps, 0_deg};
  SwerveModuleVelocity state2{1.0_mps, 0_deg};
  SwerveModuleVelocity state3{-2.0_mps, 0_deg};
  SwerveModuleVelocity state4{-2.0_mps, 0_deg};

  wpi::util::array<SwerveModuleVelocity, 4> arr{state1, state2, state3, state4};
  arr = SwerveDriveKinematics<4>::DesaturateWheelVelocities(arr, 1.0_mps);

  CHECK(arr[0].velocity.value() == Catch::Approx(0.5).margin(kEpsilon));
  CHECK(arr[1].velocity.value() == Catch::Approx(0.5).margin(kEpsilon));
  CHECK(arr[2].velocity.value() == Catch::Approx(-1.0).margin(kEpsilon));
  CHECK(arr[3].velocity.value() == Catch::Approx(-1.0).margin(kEpsilon));
}

TEST_CASE_METHOD(SwerveDriveKinematicsTest,
                 "SwerveDriveKinematicsTest TurnInPlaceInverseAccelerations",
                 "[wpimath]") {
  ChassisAccelerations accelerations{
      0_mps_sq, 0_mps_sq,
      wpi::units::radians_per_second_squared_t{2 * std::numbers::pi}};
  wpi::units::radians_per_second_t angularVelocity =
      2_rad_per_s * std::numbers::pi;
  auto [flAccel, frAccel, blAccel, brAccel] =
      m_kinematics.ToSwerveModuleAccelerations(accelerations, angularVelocity);

  // For turn-in-place with angular acceleration of 2π rad/s² and angular
  // velocity of 2π rad/s, each module has both tangential acceleration (from
  // angular acceleration) and centripetal acceleration (from angular velocity).
  // The total acceleration magnitude is approximately 678.4.
  //
  // For each swerve module at position (x, y) relative to the robot center:
  // - Distance from center: r = √(x² + y²) = √(12² + 12²) = 16.97 m
  // - Current tangential velocity: v_t = ω × r = 2π × 16.97 = 106.63 m/s
  //
  // Two acceleration components:
  // 1) Tangential acceleration (from angular acceleration α = 2π rad/s²):
  //    a_tangential = α × r = 2π × 16.97 = 106.63 m/s²
  //    Direction: perpendicular to radius vector
  //
  // 2) Centripetal acceleration (from angular velocity ω = 2π rad/s):
  //    a_centripetal = ω² × r = (2π)² × 16.97 = 668.7 m/s²
  //    Direction: toward center of rotation
  //
  // Total acceleration magnitude: |a_total| = √(a_tangential² + a_centripetal²)
  //                                         = √(106.63² + 668.7²) = 678.4 m/s²
  //
  // For module positions:
  // FL (12, 12): radius angle = 135°, tangential = 45°, centripetal = -135° →
  //              total angle = -144°
  // FR (12, -12): radius angle = 45°, tangential = -45°, centripetal = -135° →
  //               total angle = 126°
  // BL (-12, 12): radius angle = 135°, tangential = 45°, centripetal = 45° →
  //               total angle = -54°
  // BR (-12, -12): radius angle = -45°, tangential = 45°, centripetal = 135° →
  //                total angle = 36°
  //
  // The acceleration angles are not simply tangential because the centripetal
  // component from the existing angular velocity dominates and affects the
  // direction.

  double centerRadius = m_kinematics.GetModules()[0].Norm().value();
  double tangentialAccel = centerRadius * accelerations.alpha.value();  // α * r
  double centripetalAccel = centerRadius * angularVelocity.value() *
                            angularVelocity.value();  // ω² * r
  double totalAccel = std::hypot(tangentialAccel, centripetalAccel);

  std::array<Rotation2d, 4> expectedAngles;
  for (size_t i = 0; i < 4; i++) {
    Rotation2d radiusAngle = m_kinematics.GetModules()[i].Angle();

    // Tangential acceleration: perpendicular to radius (90° CCW from radius)
    Rotation2d tangentialDirection = radiusAngle + Rotation2d{90_deg};
    double tangentialX = tangentialAccel * tangentialDirection.Cos();
    double tangentialY = tangentialAccel * tangentialDirection.Sin();

    // Centripetal acceleration: toward center (opposite of radius)
    Rotation2d centripetalDirection = radiusAngle + Rotation2d{180_deg};
    double centripetalX = centripetalAccel * centripetalDirection.Cos();
    double centripetalY = centripetalAccel * centripetalDirection.Sin();

    // Vector sum of tangential and centripetal accelerations
    double totalX = tangentialX + centripetalX;
    double totalY = tangentialY + centripetalY;

    expectedAngles[i] = Rotation2d{totalX, totalY};
  }

  CHECK(totalAccel ==
        Catch::Approx(flAccel.acceleration.value()).margin(kEpsilon));
  CHECK(totalAccel ==
        Catch::Approx(frAccel.acceleration.value()).margin(kEpsilon));
  CHECK(totalAccel ==
        Catch::Approx(blAccel.acceleration.value()).margin(kEpsilon));
  CHECK(totalAccel ==
        Catch::Approx(brAccel.acceleration.value()).margin(kEpsilon));
  CHECK(expectedAngles[0].Degrees().value() ==
        Catch::Approx(flAccel.angle.Degrees().value()).margin(kEpsilon));
  CHECK(expectedAngles[1].Degrees().value() ==
        Catch::Approx(frAccel.angle.Degrees().value()).margin(kEpsilon));
  CHECK(expectedAngles[2].Degrees().value() ==
        Catch::Approx(blAccel.angle.Degrees().value()).margin(kEpsilon));
  CHECK(expectedAngles[3].Degrees().value() ==
        Catch::Approx(brAccel.angle.Degrees().value()).margin(kEpsilon));
}

TEST_CASE_METHOD(SwerveDriveKinematicsTest,
                 "SwerveDriveKinematicsTest TurnInPlaceForwardAccelerations",
                 "[wpimath]") {
  SwerveModuleAcceleration flAccel{106.629_mps_sq, 135_deg};
  SwerveModuleAcceleration frAccel{106.629_mps_sq, 45_deg};
  SwerveModuleAcceleration blAccel{106.629_mps_sq, -135_deg};
  SwerveModuleAcceleration brAccel{106.629_mps_sq, -45_deg};

  auto chassisAccelerations =
      m_kinematics.ToChassisAccelerations(flAccel, frAccel, blAccel, brAccel);

  CHECK(0.0 == Catch::Approx(chassisAccelerations.ax.value()).margin(kEpsilon));
  CHECK(0.0 == Catch::Approx(chassisAccelerations.ay.value()).margin(kEpsilon));
  CHECK(2 * std::numbers::pi ==
        Catch::Approx(chassisAccelerations.alpha.value()).margin(kEpsilon));
}

TEST_CASE_METHOD(
    SwerveDriveKinematicsTest,
    "SwerveDriveKinematicsTest OffCenterRotationInverseAccelerations",
    "[wpimath]") {
  ChassisAccelerations accelerations{0_mps_sq, 0_mps_sq, 1_rad_per_s_sq};
  // For this test, assume an angular velocity of 1 rad/s
  wpi::units::radians_per_second_t angularVelocity = 1.0_rad_per_s;
  auto [flAccel, frAccel, blAccel, brAccel] =
      m_kinematics.ToSwerveModuleAccelerations(accelerations, angularVelocity,
                                               m_fl);

  // When rotating about the front-left module position with both angular
  // acceleration (1 rad/s²) and angular velocity (1 rad/s), each module
  // experiences both tangential and centripetal accelerations that combine
  // vectorially.
  //
  // Center of rotation: FL module at (12, 12) inches
  // Module positions relative to center of rotation:
  // - FL: (0, 0) - at center of rotation
  // - FR: (0, -24) - 24 m south of center
  // - BL: (-24, 0) - 24 m west of center
  // - BR: (-24, -24) - distance = √(24² + 24²) = 33.94 m southwest
  //
  // For each module at distance r from center of rotation:
  // 1) Tangential acceleration: a_t = α × r = 1 × r
  //    Direction: perpendicular to radius vector (90° CCW from radius)
  //
  // 2) Centripetal acceleration: a_c = ω² × r = 1² × r = r
  //    Direction: toward center of rotation

  std::array<double, 4> expectedAccelerations;
  std::array<Rotation2d, 4> expectedAngles;

  for (size_t i = 0; i < 4; i++) {
    Translation2d relativePos = m_kinematics.GetModules()[i] - m_fl;
    double r = relativePos.Norm().value();

    if (r < 1e-9) {
      expectedAccelerations[i] = 0.0;  // No acceleration at center of rotation
      expectedAngles[i] =
          Rotation2d{};  // Angle is undefined at center of rotation
    } else {
      double tangentialAccel =
          r * accelerations.alpha.value();  // α * r = 1 * r
      double centripetalAccel = r * angularVelocity.value() *
                                angularVelocity.value();  // ω² * r = 1 * r
      expectedAccelerations[i] = std::hypot(tangentialAccel, centripetalAccel);

      Rotation2d radiusAngle{(relativePos.X().value()),
                             (relativePos.Y().value())};

      // Tangential acceleration: perpendicular to radius (90° CCW from radius)
      Rotation2d tangentialDirection = radiusAngle + Rotation2d{90_deg};
      double tangentialX = tangentialDirection.Cos() * r;  // α * r = 1 * r
      double tangentialY = tangentialDirection.Sin() * r;

      // Centripetal acceleration: toward center (opposite of radius)
      Rotation2d centripetalDirection = radiusAngle + Rotation2d{180_deg};
      double centripetalX = centripetalDirection.Cos() * r;  // ω² * r = 1 * r
      double centripetalY = centripetalDirection.Sin() * r;

      // Vector sum of tangential and centripetal accelerations
      double totalX = tangentialX + centripetalX;
      double totalY = tangentialY + centripetalY;

      expectedAngles[i] = Rotation2d{totalX, totalY};
    }
  }

  CHECK(expectedAccelerations[0] ==
        Catch::Approx(flAccel.acceleration.value()).margin(kEpsilon));
  CHECK(expectedAccelerations[1] ==
        Catch::Approx(frAccel.acceleration.value()).margin(kEpsilon));
  CHECK(expectedAccelerations[2] ==
        Catch::Approx(blAccel.acceleration.value()).margin(kEpsilon));
  CHECK(expectedAccelerations[3] ==
        Catch::Approx(brAccel.acceleration.value()).margin(kEpsilon));
  CHECK(expectedAngles[0].Degrees().value() ==
        Catch::Approx(flAccel.angle.Degrees().value()).margin(kEpsilon));
  CHECK(expectedAngles[1].Degrees().value() ==
        Catch::Approx(frAccel.angle.Degrees().value()).margin(kEpsilon));
  CHECK(expectedAngles[2].Degrees().value() ==
        Catch::Approx(blAccel.angle.Degrees().value()).margin(kEpsilon));
  CHECK(expectedAngles[3].Degrees().value() ==
        Catch::Approx(brAccel.angle.Degrees().value()).margin(kEpsilon));
}
