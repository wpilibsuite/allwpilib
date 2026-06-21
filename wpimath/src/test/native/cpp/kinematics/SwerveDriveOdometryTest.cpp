// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/SwerveDriveOdometry.hpp"

#include <limits>
#include <random>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/kinematics/SwerveDriveKinematics.hpp"
#include "wpi/math/trajectory/Trajectory.hpp"
#include "wpi/math/trajectory/TrajectoryConfig.hpp"
#include "wpi/math/trajectory/TrajectoryGenerator.hpp"

using namespace wpi::math;

static constexpr double kEpsilon = 0.01;

class SwerveDriveOdometryTest {
 protected:
  Translation2d m_fl{12_m, 12_m};
  Translation2d m_fr{12_m, -12_m};
  Translation2d m_bl{-12_m, 12_m};
  Translation2d m_br{-12_m, -12_m};

  SwerveDriveKinematics<4> m_kinematics{m_fl, m_fr, m_bl, m_br};
  SwerveModulePosition zero;
  SwerveDriveOdometry<4> m_odometry{
      m_kinematics, 0_rad, {zero, zero, zero, zero}};
};

TEST_CASE_METHOD(SwerveDriveOdometryTest,
                 "SwerveDriveOdometryTest TwoIterations", "[wpimath]") {
  SwerveModulePosition position{0.5_m, 0_deg};

  m_odometry.ResetPosition(0_rad, {zero, zero, zero, zero}, Pose2d{});

  m_odometry.Update(0_deg, {zero, zero, zero, zero});

  auto pose =
      m_odometry.Update(0_deg, {position, position, position, position});

  CHECK(0.5 == Catch::Approx(pose.X().value()).margin(kEpsilon));
  CHECK(0.0 == Catch::Approx(pose.Y().value()).margin(kEpsilon));
  CHECK(0.0 ==
        Catch::Approx(pose.Rotation().Degrees().value()).margin(kEpsilon));
}

TEST_CASE_METHOD(SwerveDriveOdometryTest,
                 "SwerveDriveOdometryTest 90DegreeTurn", "[wpimath]") {
  SwerveModulePosition fl{18.85_m, 90_deg};
  SwerveModulePosition fr{42.15_m, 26.565_deg};
  SwerveModulePosition bl{18.85_m, -90_deg};
  SwerveModulePosition br{42.15_m, -26.565_deg};

  m_odometry.ResetPosition(0_rad, {zero, zero, zero, zero}, Pose2d{});
  auto pose = m_odometry.Update(90_deg, {fl, fr, bl, br});

  CHECK(12.0 == Catch::Approx(pose.X().value()).margin(kEpsilon));
  CHECK(12.0 == Catch::Approx(pose.Y().value()).margin(kEpsilon));
  CHECK(90.0 ==
        Catch::Approx(pose.Rotation().Degrees().value()).margin(kEpsilon));
}

TEST_CASE_METHOD(SwerveDriveOdometryTest,
                 "SwerveDriveOdometryTest GyroAngleReset", "[wpimath]") {
  m_odometry.ResetPosition(90_deg, {zero, zero, zero, zero}, Pose2d{});

  SwerveModulePosition position{0.5_m, 0_deg};

  auto pose =
      m_odometry.Update(90_deg, {position, position, position, position});

  CHECK(0.5 == Catch::Approx(pose.X().value()).margin(kEpsilon));
  CHECK(0.0 == Catch::Approx(pose.Y().value()).margin(kEpsilon));
  CHECK(0.0 ==
        Catch::Approx(pose.Rotation().Degrees().value()).margin(kEpsilon));
}

TEST_CASE_METHOD(SwerveDriveOdometryTest,
                 "SwerveDriveOdometryTest ResetTranslation", "[wpimath]") {
  wpi::util::array<SwerveModulePosition, 4> wheelPositions{zero, zero, zero,
                                                           zero};

  m_odometry.ResetPosition(0_deg, wheelPositions, Pose2d{});
  auto pose = m_odometry.Update(30_deg, wheelPositions);

  CHECK(0.0 == Catch::Approx(pose.X().value()).margin(kEpsilon));
  CHECK(0.0 == Catch::Approx(pose.Y().value()).margin(kEpsilon));
  CHECK(30.0 ==
        Catch::Approx(pose.Rotation().Degrees().value()).margin(kEpsilon));

  m_odometry.ResetTranslation({0.5_m, 0_m});
  pose = m_odometry.Update(30_deg, wheelPositions);

  CHECK(0.5 == Catch::Approx(pose.X().value()).margin(kEpsilon));
  CHECK(0.0 == Catch::Approx(pose.Y().value()).margin(kEpsilon));
  CHECK(30.0 ==
        Catch::Approx(pose.Rotation().Degrees().value()).margin(kEpsilon));
}

TEST_CASE_METHOD(SwerveDriveOdometryTest,
                 "SwerveDriveOdometryTest ResetRotation", "[wpimath]") {
  wpi::util::array<SwerveModulePosition, 4> wheelPositions{zero, zero, zero,
                                                           zero};

  m_odometry.ResetPosition(0_deg, wheelPositions, Pose2d{0.5_m, 0_m, 0_rad});
  auto pose = m_odometry.Update(30_deg, wheelPositions);

  CHECK(0.5 == Catch::Approx(pose.X().value()).margin(kEpsilon));
  CHECK(0.0 == Catch::Approx(pose.Y().value()).margin(kEpsilon));
  CHECK(30.0 ==
        Catch::Approx(pose.Rotation().Degrees().value()).margin(kEpsilon));

  m_odometry.ResetRotation(90_deg);
  pose = m_odometry.Update(30_deg, wheelPositions);

  CHECK(0.5 == Catch::Approx(pose.X().value()).margin(kEpsilon));
  CHECK(0.0 == Catch::Approx(pose.Y().value()).margin(kEpsilon));
  CHECK(90.0 ==
        Catch::Approx(pose.Rotation().Degrees().value()).margin(kEpsilon));
}

TEST_CASE_METHOD(SwerveDriveOdometryTest,
                 "SwerveDriveOdometryTest AccuracyFacingTrajectory",
                 "[wpimath]") {
  SwerveDriveKinematics<4> kinematics{
      Translation2d{1_m, 1_m}, Translation2d{1_m, -1_m},
      Translation2d{-1_m, -1_m}, Translation2d{-1_m, 1_m}};

  SwerveDriveOdometry<4> odometry{
      kinematics, wpi::math::Rotation2d{}, {zero, zero, zero, zero}};

  SwerveModulePosition fl;
  SwerveModulePosition fr;
  SwerveModulePosition bl;
  SwerveModulePosition br;

  Trajectory trajectory = TrajectoryGenerator::GenerateTrajectory(
      std::vector{Pose2d{0_m, 0_m, 45_deg}, Pose2d{3_m, 0_m, -90_deg},
                  Pose2d{0_m, 0_m, 135_deg}, Pose2d{-3_m, 0_m, -90_deg},
                  Pose2d{0_m, 0_m, 45_deg}},
      TrajectoryConfig(5.0_mps, 2.0_mps_sq));

  std::default_random_engine generator;
  std::normal_distribution<double> distribution(0.0, 1.0);

  wpi::units::second_t dt = 20_ms;
  wpi::units::second_t t = 0_s;

  double maxError = -std::numeric_limits<double>::max();
  double errorSum = 0;

  while (t < trajectory.TotalTime()) {
    Trajectory::State groundTruthState = trajectory.Sample(t);

    auto moduleVelocities = kinematics.ToSwerveModuleVelocities(
        {groundTruthState.velocity, 0_mps,
         groundTruthState.velocity * groundTruthState.curvature});

    fl.distance += moduleVelocities[0].velocity * dt;
    fr.distance += moduleVelocities[1].velocity * dt;
    bl.distance += moduleVelocities[2].velocity * dt;
    br.distance += moduleVelocities[3].velocity * dt;

    fl.angle = moduleVelocities[0].angle;
    fr.angle = moduleVelocities[1].angle;
    bl.angle = moduleVelocities[2].angle;
    br.angle = moduleVelocities[3].angle;

    auto xhat = odometry.Update(
        groundTruthState.pose.Rotation() +
            wpi::math::Rotation2d{distribution(generator) * 0.05_rad},
        {fl, fr, bl, br});
    double error = groundTruthState.pose.Translation()
                       .Distance(xhat.Translation())
                       .value();

    if (error > maxError) {
      maxError = error;
    }
    errorSum += error;

    t += dt;
  }

  CHECK(errorSum / (trajectory.TotalTime().value() / dt.value()) < 0.05);
  CHECK(maxError < 0.125);
}

TEST_CASE_METHOD(SwerveDriveOdometryTest,
                 "SwerveDriveOdometryTest AccuracyFacingXAxis", "[wpimath]") {
  SwerveDriveKinematics<4> kinematics{
      Translation2d{1_m, 1_m}, Translation2d{1_m, -1_m},
      Translation2d{-1_m, -1_m}, Translation2d{-1_m, 1_m}};

  SwerveDriveOdometry<4> odometry{
      kinematics, wpi::math::Rotation2d{}, {zero, zero, zero, zero}};

  SwerveModulePosition fl;
  SwerveModulePosition fr;
  SwerveModulePosition bl;
  SwerveModulePosition br;

  Trajectory trajectory = TrajectoryGenerator::GenerateTrajectory(
      std::vector{Pose2d{0_m, 0_m, 45_deg}, Pose2d{3_m, 0_m, -90_deg},
                  Pose2d{0_m, 0_m, 135_deg}, Pose2d{-3_m, 0_m, -90_deg},
                  Pose2d{0_m, 0_m, 45_deg}},
      TrajectoryConfig(5.0_mps, 2.0_mps_sq));

  std::default_random_engine generator;
  std::normal_distribution<double> distribution(0.0, 1.0);

  wpi::units::second_t dt = 20_ms;
  wpi::units::second_t t = 0_s;

  double maxError = -std::numeric_limits<double>::max();
  double errorSum = 0;

  while (t < trajectory.TotalTime()) {
    Trajectory::State groundTruthState = trajectory.Sample(t);

    fl.distance += groundTruthState.velocity * dt +
                   0.5 * groundTruthState.acceleration * dt * dt;
    fr.distance += groundTruthState.velocity * dt +
                   0.5 * groundTruthState.acceleration * dt * dt;
    bl.distance += groundTruthState.velocity * dt +
                   0.5 * groundTruthState.acceleration * dt * dt;
    br.distance += groundTruthState.velocity * dt +
                   0.5 * groundTruthState.acceleration * dt * dt;

    fl.angle = groundTruthState.pose.Rotation();
    fr.angle = groundTruthState.pose.Rotation();
    bl.angle = groundTruthState.pose.Rotation();
    br.angle = groundTruthState.pose.Rotation();

    auto xhat = odometry.Update(
        wpi::math::Rotation2d{distribution(generator) * 0.05_rad},
        {fl, fr, bl, br});
    double error = groundTruthState.pose.Translation()
                       .Distance(xhat.Translation())
                       .value();

    if (error > maxError) {
      maxError = error;
    }
    errorSum += error;

    t += dt;
  }

  CHECK(errorSum / (trajectory.TotalTime().value() / dt.value()) < 0.06);
  CHECK(maxError < 0.125);
}
