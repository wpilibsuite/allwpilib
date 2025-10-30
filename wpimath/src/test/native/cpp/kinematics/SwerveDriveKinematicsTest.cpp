// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <numbers>

#include <gtest/gtest.h>

#include "frc/geometry/Translation2d.h"
#include "frc/kinematics/ChassisAccelerations.h"
#include "frc/kinematics/SwerveDriveKinematics.h"
#include "frc/kinematics/SwerveModuleAccelerations.h"
#include "units/angular_velocity.h"

using namespace frc;

static constexpr double kEpsilon = 0.1;

class SwerveDriveKinematicsTest : public ::testing::Test {
 protected:
  Translation2d m_fl{12_m, 12_m};
  Translation2d m_fr{12_m, -12_m};
  Translation2d m_bl{-12_m, 12_m};
  Translation2d m_br{-12_m, -12_m};

  SwerveDriveKinematics<4> m_kinematics{m_fl, m_fr, m_bl, m_br};
};

TEST_F(SwerveDriveKinematicsTest, StraightLineInverseKinematics) {
  ChassisSpeeds speeds{5.0_mps, 0.0_mps, 0.0_rad_per_s};

  auto [fl, fr, bl, br] = m_kinematics.ToSwerveModuleStates(speeds);

  EXPECT_NEAR(fl.speed.value(), 5.0, kEpsilon);
  EXPECT_NEAR(fr.speed.value(), 5.0, kEpsilon);
  EXPECT_NEAR(bl.speed.value(), 5.0, kEpsilon);
  EXPECT_NEAR(br.speed.value(), 5.0, kEpsilon);

  EXPECT_NEAR(fl.angle.Radians().value(), 0.0, kEpsilon);
  EXPECT_NEAR(fr.angle.Radians().value(), 0.0, kEpsilon);
  EXPECT_NEAR(bl.angle.Radians().value(), 0.0, kEpsilon);
  EXPECT_NEAR(br.angle.Radians().value(), 0.0, kEpsilon);
}

TEST_F(SwerveDriveKinematicsTest, StraightLineForwardKinematics) {
  SwerveModuleState state{5.0_mps, 0_deg};

  auto chassisSpeeds = m_kinematics.ToChassisSpeeds(state, state, state, state);

  EXPECT_NEAR(chassisSpeeds.vx.value(), 5.0, kEpsilon);
  EXPECT_NEAR(chassisSpeeds.vy.value(), 0.0, kEpsilon);
  EXPECT_NEAR(chassisSpeeds.omega.value(), 0.0, kEpsilon);
}

TEST_F(SwerveDriveKinematicsTest, StraightLineForwardKinematicsWithDeltas) {
  SwerveModulePosition delta{5.0_m, 0_deg};

  auto twist = m_kinematics.ToTwist2d(delta, delta, delta, delta);

  EXPECT_NEAR(twist.dx.value(), 5.0, kEpsilon);
  EXPECT_NEAR(twist.dy.value(), 0.0, kEpsilon);
  EXPECT_NEAR(twist.dtheta.value(), 0.0, kEpsilon);
}

TEST_F(SwerveDriveKinematicsTest, StraightStrafeInverseKinematics) {
  ChassisSpeeds speeds{0_mps, 5_mps, 0_rad_per_s};
  auto [fl, fr, bl, br] = m_kinematics.ToSwerveModuleStates(speeds);

  EXPECT_NEAR(fl.speed.value(), 5.0, kEpsilon);
  EXPECT_NEAR(fr.speed.value(), 5.0, kEpsilon);
  EXPECT_NEAR(bl.speed.value(), 5.0, kEpsilon);
  EXPECT_NEAR(br.speed.value(), 5.0, kEpsilon);

  EXPECT_NEAR(fl.angle.Degrees().value(), 90.0, kEpsilon);
  EXPECT_NEAR(fr.angle.Degrees().value(), 90.0, kEpsilon);
  EXPECT_NEAR(bl.angle.Degrees().value(), 90.0, kEpsilon);
  EXPECT_NEAR(br.angle.Degrees().value(), 90.0, kEpsilon);
}

TEST_F(SwerveDriveKinematicsTest, StraightStrafeForwardKinematics) {
  SwerveModuleState state{5_mps, 90_deg};
  auto chassisSpeeds = m_kinematics.ToChassisSpeeds(state, state, state, state);

  EXPECT_NEAR(chassisSpeeds.vx.value(), 0.0, kEpsilon);
  EXPECT_NEAR(chassisSpeeds.vy.value(), 5.0, kEpsilon);
  EXPECT_NEAR(chassisSpeeds.omega.value(), 0.0, kEpsilon);
}

TEST_F(SwerveDriveKinematicsTest, StraightStrafeForwardKinematicsWithDeltas) {
  SwerveModulePosition delta{5_m, 90_deg};

  auto twist = m_kinematics.ToTwist2d(delta, delta, delta, delta);

  EXPECT_NEAR(twist.dx.value(), 0.0, kEpsilon);
  EXPECT_NEAR(twist.dy.value(), 5.0, kEpsilon);
  EXPECT_NEAR(twist.dtheta.value(), 0.0, kEpsilon);
}

TEST_F(SwerveDriveKinematicsTest, TurnInPlaceInverseKinematics) {
  ChassisSpeeds speeds{0_mps, 0_mps,
                       units::radians_per_second_t{2 * std::numbers::pi}};
  auto [fl, fr, bl, br] = m_kinematics.ToSwerveModuleStates(speeds);

  EXPECT_NEAR(fl.speed.value(), 106.63, kEpsilon);
  EXPECT_NEAR(fr.speed.value(), 106.63, kEpsilon);
  EXPECT_NEAR(bl.speed.value(), 106.63, kEpsilon);
  EXPECT_NEAR(br.speed.value(), 106.63, kEpsilon);

  EXPECT_NEAR(fl.angle.Degrees().value(), 135.0, kEpsilon);
  EXPECT_NEAR(fr.angle.Degrees().value(), 45.0, kEpsilon);
  EXPECT_NEAR(bl.angle.Degrees().value(), -135.0, kEpsilon);
  EXPECT_NEAR(br.angle.Degrees().value(), -45.0, kEpsilon);
}

TEST_F(SwerveDriveKinematicsTest, ConserveWheelAngle) {
  ChassisSpeeds speeds{0_mps, 0_mps,
                       units::radians_per_second_t{2 * std::numbers::pi}};
  m_kinematics.ToSwerveModuleStates(speeds);
  auto [fl, fr, bl, br] = m_kinematics.ToSwerveModuleStates(ChassisSpeeds{});

  EXPECT_NEAR(fl.speed.value(), 0.0, kEpsilon);
  EXPECT_NEAR(fr.speed.value(), 0.0, kEpsilon);
  EXPECT_NEAR(bl.speed.value(), 0.0, kEpsilon);
  EXPECT_NEAR(br.speed.value(), 0.0, kEpsilon);

  EXPECT_NEAR(fl.angle.Degrees().value(), 135.0, kEpsilon);
  EXPECT_NEAR(fr.angle.Degrees().value(), 45.0, kEpsilon);
  EXPECT_NEAR(bl.angle.Degrees().value(), -135.0, kEpsilon);
  EXPECT_NEAR(br.angle.Degrees().value(), -45.0, kEpsilon);
}
TEST_F(SwerveDriveKinematicsTest, ResetWheelAngle) {
  Rotation2d fl = {0_deg};
  Rotation2d fr = {90_deg};
  Rotation2d bl = {180_deg};
  Rotation2d br = {270_deg};
  m_kinematics.ResetHeadings(fl, fr, bl, br);
  auto [flMod, frMod, blMod, brMod] =
      m_kinematics.ToSwerveModuleStates(ChassisSpeeds{});

  EXPECT_NEAR(flMod.speed.value(), 0.0, kEpsilon);
  EXPECT_NEAR(frMod.speed.value(), 0.0, kEpsilon);
  EXPECT_NEAR(blMod.speed.value(), 0.0, kEpsilon);
  EXPECT_NEAR(brMod.speed.value(), 0.0, kEpsilon);

  EXPECT_NEAR(flMod.angle.Degrees().value(), 0.0, kEpsilon);
  EXPECT_NEAR(frMod.angle.Degrees().value(), 90.0, kEpsilon);
  EXPECT_NEAR(blMod.angle.Degrees().value(), 180.0, kEpsilon);
  EXPECT_NEAR(brMod.angle.Degrees().value(), -90.0, kEpsilon);
}

TEST_F(SwerveDriveKinematicsTest, TurnInPlaceForwardKinematics) {
  SwerveModuleState fl{106.629_mps, 135_deg};
  SwerveModuleState fr{106.629_mps, 45_deg};
  SwerveModuleState bl{106.629_mps, -135_deg};
  SwerveModuleState br{106.629_mps, -45_deg};

  auto chassisSpeeds = m_kinematics.ToChassisSpeeds(fl, fr, bl, br);

  EXPECT_NEAR(chassisSpeeds.vx.value(), 0.0, kEpsilon);
  EXPECT_NEAR(chassisSpeeds.vy.value(), 0.0, kEpsilon);
  EXPECT_NEAR(chassisSpeeds.omega.value(), 2 * std::numbers::pi, kEpsilon);
}

TEST_F(SwerveDriveKinematicsTest, TurnInPlaceForwardKinematicsWithDeltas) {
  SwerveModulePosition fl{106.629_m, 135_deg};
  SwerveModulePosition fr{106.629_m, 45_deg};
  SwerveModulePosition bl{106.629_m, -135_deg};
  SwerveModulePosition br{106.629_m, -45_deg};

  auto twist = m_kinematics.ToTwist2d(fl, fr, bl, br);

  EXPECT_NEAR(twist.dx.value(), 0.0, kEpsilon);
  EXPECT_NEAR(twist.dy.value(), 0.0, kEpsilon);
  EXPECT_NEAR(twist.dtheta.value(), 2 * std::numbers::pi, kEpsilon);
}

TEST_F(SwerveDriveKinematicsTest, OffCenterCORRotationInverseKinematics) {
  ChassisSpeeds speeds{0_mps, 0_mps,
                       units::radians_per_second_t{2 * std::numbers::pi}};
  auto [fl, fr, bl, br] = m_kinematics.ToSwerveModuleStates(speeds, m_fl);

  EXPECT_NEAR(fl.speed.value(), 0.0, kEpsilon);
  EXPECT_NEAR(fr.speed.value(), 150.796, kEpsilon);
  EXPECT_NEAR(bl.speed.value(), 150.796, kEpsilon);
  EXPECT_NEAR(br.speed.value(), 213.258, kEpsilon);

  EXPECT_NEAR(fl.angle.Degrees().value(), 0.0, kEpsilon);
  EXPECT_NEAR(fr.angle.Degrees().value(), 0.0, kEpsilon);
  EXPECT_NEAR(bl.angle.Degrees().value(), -90.0, kEpsilon);
  EXPECT_NEAR(br.angle.Degrees().value(), -45.0, kEpsilon);
}

TEST_F(SwerveDriveKinematicsTest, OffCenterCORRotationForwardKinematics) {
  SwerveModuleState fl{0.0_mps, 0_deg};
  SwerveModuleState fr{150.796_mps, 0_deg};
  SwerveModuleState bl{150.796_mps, -90_deg};
  SwerveModuleState br{213.258_mps, -45_deg};

  auto chassisSpeeds = m_kinematics.ToChassisSpeeds(fl, fr, bl, br);

  EXPECT_NEAR(chassisSpeeds.vx.value(), 75.398, kEpsilon);
  EXPECT_NEAR(chassisSpeeds.vy.value(), -75.398, kEpsilon);
  EXPECT_NEAR(chassisSpeeds.omega.value(), 2 * std::numbers::pi, kEpsilon);
}

TEST_F(SwerveDriveKinematicsTest,
       OffCenterCORRotationForwardKinematicsWithDeltas) {
  SwerveModulePosition fl{0.0_m, 0_deg};
  SwerveModulePosition fr{150.796_m, 0_deg};
  SwerveModulePosition bl{150.796_m, -90_deg};
  SwerveModulePosition br{213.258_m, -45_deg};

  auto twist = m_kinematics.ToTwist2d(fl, fr, bl, br);

  EXPECT_NEAR(twist.dx.value(), 75.398, kEpsilon);
  EXPECT_NEAR(twist.dy.value(), -75.398, kEpsilon);
  EXPECT_NEAR(twist.dtheta.value(), 2 * std::numbers::pi, kEpsilon);
}

TEST_F(SwerveDriveKinematicsTest,
       OffCenterCORRotationAndTranslationInverseKinematics) {
  ChassisSpeeds speeds{0_mps, 3.0_mps, 1.5_rad_per_s};
  auto [fl, fr, bl, br] =
      m_kinematics.ToSwerveModuleStates(speeds, Translation2d{24_m, 0_m});

  EXPECT_NEAR(fl.speed.value(), 23.43, kEpsilon);
  EXPECT_NEAR(fr.speed.value(), 23.43, kEpsilon);
  EXPECT_NEAR(bl.speed.value(), 54.08, kEpsilon);
  EXPECT_NEAR(br.speed.value(), 54.08, kEpsilon);

  EXPECT_NEAR(fl.angle.Degrees().value(), -140.19, kEpsilon);
  EXPECT_NEAR(fr.angle.Degrees().value(), -39.81, kEpsilon);
  EXPECT_NEAR(bl.angle.Degrees().value(), -109.44, kEpsilon);
  EXPECT_NEAR(br.angle.Degrees().value(), -70.56, kEpsilon);
}

TEST_F(SwerveDriveKinematicsTest,
       OffCenterCORRotationAndTranslationForwardKinematics) {
  SwerveModuleState fl{23.43_mps, -140.19_deg};
  SwerveModuleState fr{23.43_mps, -39.81_deg};
  SwerveModuleState bl{54.08_mps, -109.44_deg};
  SwerveModuleState br{54.08_mps, -70.56_deg};

  auto chassisSpeeds = m_kinematics.ToChassisSpeeds(fl, fr, bl, br);

  EXPECT_NEAR(chassisSpeeds.vx.value(), 0.0, kEpsilon);
  EXPECT_NEAR(chassisSpeeds.vy.value(), -33.0, kEpsilon);
  EXPECT_NEAR(chassisSpeeds.omega.value(), 1.5, kEpsilon);
}

TEST_F(SwerveDriveKinematicsTest,
       OffCenterCORRotationAndTranslationForwardKinematicsWithDeltas) {
  SwerveModulePosition fl{23.43_m, -140.19_deg};
  SwerveModulePosition fr{23.43_m, -39.81_deg};
  SwerveModulePosition bl{54.08_m, -109.44_deg};
  SwerveModulePosition br{54.08_m, -70.56_deg};

  auto twist = m_kinematics.ToTwist2d(fl, fr, bl, br);

  EXPECT_NEAR(twist.dx.value(), 0.0, kEpsilon);
  EXPECT_NEAR(twist.dy.value(), -33.0, kEpsilon);
  EXPECT_NEAR(twist.dtheta.value(), 1.5, kEpsilon);
}

TEST_F(SwerveDriveKinematicsTest, Desaturate) {
  SwerveModuleState state1{5.0_mps, 0_deg};
  SwerveModuleState state2{6.0_mps, 0_deg};
  SwerveModuleState state3{4.0_mps, 0_deg};
  SwerveModuleState state4{7.0_mps, 0_deg};

  wpi::array<SwerveModuleState, 4> arr{state1, state2, state3, state4};
  SwerveDriveKinematics<4>::DesaturateWheelSpeeds(&arr, 5.5_mps);

  double kFactor = 5.5 / 7.0;

  EXPECT_NEAR(arr[0].speed.value(), 5.0 * kFactor, kEpsilon);
  EXPECT_NEAR(arr[1].speed.value(), 6.0 * kFactor, kEpsilon);
  EXPECT_NEAR(arr[2].speed.value(), 4.0 * kFactor, kEpsilon);
  EXPECT_NEAR(arr[3].speed.value(), 7.0 * kFactor, kEpsilon);
}

TEST_F(SwerveDriveKinematicsTest, DesaturateSmooth) {
  SwerveModuleState state1{5.0_mps, 0_deg};
  SwerveModuleState state2{6.0_mps, 0_deg};
  SwerveModuleState state3{4.0_mps, 0_deg};
  SwerveModuleState state4{7.0_mps, 0_deg};

  wpi::array<SwerveModuleState, 4> arr{state1, state2, state3, state4};
  SwerveDriveKinematics<4>::DesaturateWheelSpeeds(
      &arr, m_kinematics.ToChassisSpeeds(arr), 5.5_mps, 5.5_mps, 3.5_rad_per_s);

  double kFactor = 5.5 / 7.0;

  EXPECT_NEAR(arr[0].speed.value(), 5.0 * kFactor, kEpsilon);
  EXPECT_NEAR(arr[1].speed.value(), 6.0 * kFactor, kEpsilon);
  EXPECT_NEAR(arr[2].speed.value(), 4.0 * kFactor, kEpsilon);
  EXPECT_NEAR(arr[3].speed.value(), 7.0 * kFactor, kEpsilon);
}

TEST_F(SwerveDriveKinematicsTest, DesaturateNegativeSpeed) {
  SwerveModuleState state1{1.0_mps, 0_deg};
  SwerveModuleState state2{1.0_mps, 0_deg};
  SwerveModuleState state3{-2.0_mps, 0_deg};
  SwerveModuleState state4{-2.0_mps, 0_deg};

  wpi::array<SwerveModuleState, 4> arr{state1, state2, state3, state4};
  SwerveDriveKinematics<4>::DesaturateWheelSpeeds(&arr, 1.0_mps);

  EXPECT_NEAR(arr[0].speed.value(), 0.5, kEpsilon);
  EXPECT_NEAR(arr[1].speed.value(), 0.5, kEpsilon);
  EXPECT_NEAR(arr[2].speed.value(), -1.0, kEpsilon);
  EXPECT_NEAR(arr[3].speed.value(), -1.0, kEpsilon);
}

TEST_F(SwerveDriveKinematicsTest, TurnInPlaceInverseAccelerations) {
  ChassisAccelerations accelerations{
      0_mps_sq, 0_mps_sq,
      units::radians_per_second_squared_t{2 * std::numbers::pi}};
  units::radians_per_second_t angularVelocity = 2_rad_per_s * std::numbers::pi;
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
  // FL (12, 12):   radius angle = 135°, tangential = 45°,  centripetal =
  // -135° → total angle = -144° FR (12, -12):  radius angle = 45°, tangential =
  // -45°, centripetal = -135° → total angle = 126° BL (-12, 12):  radius angle
  // = 135°, tangential = 45°,  centripetal = 45°   → total angle = -54° BR
  // (-12, -12): radius angle = -45°, tangential = 45°,  centripetal = 135°  →
  // total angle = 36°
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

  EXPECT_NEAR(totalAccel, flAccel.acceleration.value(), kEpsilon);
  EXPECT_NEAR(totalAccel, frAccel.acceleration.value(), kEpsilon);
  EXPECT_NEAR(totalAccel, blAccel.acceleration.value(), kEpsilon);
  EXPECT_NEAR(totalAccel, brAccel.acceleration.value(), kEpsilon);
  EXPECT_NEAR(expectedAngles[0].Degrees().value(),
              flAccel.angle.Degrees().value(), kEpsilon);
  EXPECT_NEAR(expectedAngles[1].Degrees().value(),
              frAccel.angle.Degrees().value(), kEpsilon);
  EXPECT_NEAR(expectedAngles[2].Degrees().value(),
              blAccel.angle.Degrees().value(), kEpsilon);
  EXPECT_NEAR(expectedAngles[3].Degrees().value(),
              brAccel.angle.Degrees().value(), kEpsilon);
}

TEST_F(SwerveDriveKinematicsTest, TurnInPlaceForwardAccelerations) {
  SwerveModuleAccelerations flAccel{106.629_mps_sq, 135_deg};
  SwerveModuleAccelerations frAccel{106.629_mps_sq, 45_deg};
  SwerveModuleAccelerations blAccel{106.629_mps_sq, -135_deg};
  SwerveModuleAccelerations brAccel{106.629_mps_sq, -45_deg};

  auto chassisAccelerations =
      m_kinematics.ToChassisAccelerations(flAccel, frAccel, blAccel, brAccel);

  EXPECT_NEAR(0.0, chassisAccelerations.ax.value(), kEpsilon);
  EXPECT_NEAR(0.0, chassisAccelerations.ay.value(), kEpsilon);
  EXPECT_NEAR(2 * std::numbers::pi, chassisAccelerations.alpha.value(),
              kEpsilon);
}

TEST_F(SwerveDriveKinematicsTest, OffCenterRotationInverseAccelerations) {
  ChassisAccelerations accelerations{0_mps_sq, 0_mps_sq, 1_rad_per_s_sq};
  // For this test, assume an angular velocity of 1 rad/s
  units::radians_per_second_t angularVelocity = 1.0_rad_per_s;
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

  EXPECT_NEAR(expectedAccelerations[0], flAccel.acceleration.value(), kEpsilon);
  EXPECT_NEAR(expectedAccelerations[1], frAccel.acceleration.value(), kEpsilon);
  EXPECT_NEAR(expectedAccelerations[2], blAccel.acceleration.value(), kEpsilon);
  EXPECT_NEAR(expectedAccelerations[3], brAccel.acceleration.value(), kEpsilon);
  EXPECT_NEAR(expectedAngles[0].Degrees().value(),
              flAccel.angle.Degrees().value(), kEpsilon);
  EXPECT_NEAR(expectedAngles[1].Degrees().value(),
              frAccel.angle.Degrees().value(), kEpsilon);
  EXPECT_NEAR(expectedAngles[2].Degrees().value(),
              blAccel.angle.Degrees().value(), kEpsilon);
  EXPECT_NEAR(expectedAngles[3].Degrees().value(),
              brAccel.angle.Degrees().value(), kEpsilon);
}
