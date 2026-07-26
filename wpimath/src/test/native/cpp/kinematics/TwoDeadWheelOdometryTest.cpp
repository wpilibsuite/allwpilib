// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/TwoDeadWheelOdometry.hpp"

#include <limits>
#include <random>
#include <vector>

#include <gtest/gtest.h>

#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/geometry/Rotation2d.hpp"
#include "wpi/math/geometry/Translation2d.hpp"
#include "wpi/math/trajectory/DrivetrainSplineSample.hpp"
#include "wpi/math/trajectory/DrivetrainSplineTrajectory.hpp"
#include "wpi/math/trajectory/DrivetrainSplineTrajectoryGenerator.hpp"
#include "wpi/math/trajectory/TrajectoryConfig.hpp"
#include "wpi/units/acceleration.hpp"
#include "wpi/units/angle.hpp"
#include "wpi/units/angular_velocity.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/time.hpp"
#include "wpi/units/velocity.hpp"

using namespace wpi::math;

class TwoDeadWheelOdometryTest : public ::testing::Test {
 protected:
  wpi::units::meter_t m_xWheelYPos = 1_m;
  wpi::units::meter_t m_yWheelXPos = 1_m;

  TwoDeadWheelOdometry odometry{m_xWheelYPos, m_yWheelXPos, 0_m,
                                0_m,          0_rad,        Pose2d{}};

  Matrixd<2, 3> m_inverseKinematicsMatrix = Matrixd<2, 3>{
      {1, 0, -m_xWheelYPos.value()}, {0, 1, m_yWheelXPos.value()}};
};

TEST_F(TwoDeadWheelOdometryTest, MultipleConsecutiveUpdates) {
  odometry.ResetPosition(1_m, 1_m, 0_rad, Pose2d{});

  odometry.Update(1_m, 1_m, 0_rad);
  auto secondPose = odometry.Update(1_m, 1_m, 0_rad);

  EXPECT_NEAR(secondPose.X().value(), 0.0, 0.01);
  EXPECT_NEAR(secondPose.Y().value(), 0.0, 0.01);
  EXPECT_NEAR(secondPose.Rotation().Radians().value(), 0.0, 0.01);
}

TEST_F(TwoDeadWheelOdometryTest, TwoIterations) {
  odometry.ResetPosition(0_m, 0_m, 0_rad, Pose2d{});

  odometry.Update(0_m, 0_m, 0_rad);
  auto pose = odometry.Update(0.1_m, 0_m, 0_rad);

  EXPECT_NEAR(pose.X().value(), 0.1, 0.01);
  EXPECT_NEAR(pose.Y().value(), 0.0, 0.01);
  EXPECT_NEAR(pose.Rotation().Radians().value(), 0.0, 0.01);
}

TEST_F(TwoDeadWheelOdometryTest, GyroAngleReset) {
  odometry.ResetPosition(0_m, 0_m, 90_deg, Pose2d{});

  odometry.Update(1_m, 0_m, 90_deg);
  auto pose = odometry.Update(1_m, 0_m, 90_deg);

  EXPECT_NEAR(pose.X().value(), 1, 0.01);
  EXPECT_NEAR(pose.Y().value(), 0.0, 0.01);
  EXPECT_NEAR(pose.Rotation().Radians().value(), 0.0, 0.01);
}

TEST_F(TwoDeadWheelOdometryTest, StraightForwardsForwardKinematics) {
  const auto chassisVelocities =
      odometry.ToChassisVelocities(5_mps, 0_mps, 0_rad_per_s);

  EXPECT_NEAR(chassisVelocities.vx.value(), 5, 0.1);
  EXPECT_NEAR(chassisVelocities.vy.value(), 0, 0.1);
  EXPECT_NEAR(chassisVelocities.omega.value(), 0, 0.1);
}

TEST_F(TwoDeadWheelOdometryTest, StraightLeftForwardKinematics) {
  const auto chassisVelocities =
      odometry.ToChassisVelocities(0_mps, 5_mps, 0_rad_per_s);

  EXPECT_NEAR(chassisVelocities.vx.value(), 0, 0.1);
  EXPECT_NEAR(chassisVelocities.vy.value(), 5, 0.1);
  EXPECT_NEAR(chassisVelocities.omega.value(), 0, 0.1);
}

TEST_F(TwoDeadWheelOdometryTest, SpinInPlaceForwardKinematics) {
  const auto chassisVelocities =
      odometry.ToChassisVelocities(-5_mps, 5_mps, 5_rad_per_s);

  EXPECT_NEAR(chassisVelocities.vx.value(), 0, 0.1);
  EXPECT_NEAR(chassisVelocities.vy.value(), 0, 0.1);
  EXPECT_NEAR(chassisVelocities.omega.value(), 5, 0.1);
}

TEST_F(TwoDeadWheelOdometryTest, MixedMotionForwardKinematics) {
  const auto chassisVelocities =
      odometry.ToChassisVelocities(1_mps, -1_mps, 5_rad_per_s);

  EXPECT_NEAR(chassisVelocities.vx.value(), 6, 0.1);
  EXPECT_NEAR(chassisVelocities.vy.value(), -6, 0.1);
  EXPECT_NEAR(chassisVelocities.omega.value(), 5, 0.1);
}

TEST_F(TwoDeadWheelOdometryTest, AccuracyFacingTrajectory) {
  auto xWheelPos = 0_m;
  auto yWheelPos = 0_m;

  wpi::math::DrivetrainSplineTrajectory trajectory =
      wpi::math::DrivetrainSplineTrajectoryGenerator::Generate(
          std::vector{wpi::math::Pose2d{0_m, 0_m, 45_deg},
                      wpi::math::Pose2d{20_m, 20_m, -90_deg},
                      wpi::math::Pose2d{10_m, 10_m, 135_deg},
                      wpi::math::Pose2d{30_m, 30_m, -90_deg},
                      wpi::math::Pose2d{20_m, 20_m, 45_deg}},
          wpi::math::TrajectoryConfig(0.5_mps, 2.0_mps_sq));

  odometry.ResetPosition(xWheelPos, yWheelPos,
                         trajectory.InitialPose().Rotation(),
                         trajectory.InitialPose());

  std::default_random_engine generator;
  std::normal_distribution<double> distribution(0.0, 1.0);

  wpi::units::second_t dt = 20_ms;
  wpi::units::second_t t = 0_s;

  double maxError = -std::numeric_limits<double>::max();
  double errorSum = 0;

  wpi::units::meter_t odometryDistanceTravelled = 0_m;
  wpi::units::meter_t trajectoryDistanceTravelled = 0_m;

  while (t < trajectory.Duration()) {
    wpi::math::DrivetrainSplineSample groundTruthState = trajectory.SampleAt(t);

    trajectoryDistanceTravelled +=
        groundTruthState.ForwardVelocity() * dt +
        0.5 * groundTruthState.ForwardAcceleration() * dt * dt;

    Eigen::Vector2d wheelVelocities =
        m_inverseKinematicsMatrix *
        Eigen::Vector3d{groundTruthState.ForwardVelocity().value(), 0,
                        groundTruthState.ForwardVelocity().value() *
                            groundTruthState.curvature.value()};

    auto xWheelVel = wpi::units::meters_per_second_t{wheelVelocities(0, 0)} +
                     distribution(generator) * 0.05_mps;
    auto yWheelVel = wpi::units::meters_per_second_t{wheelVelocities(1, 0)} +
                     distribution(generator) * 0.05_mps;

    xWheelPos += xWheelVel * dt;
    yWheelPos += yWheelVel * dt;

    auto lastPose = odometry.GetPose();

    auto xhat = odometry.Update(
        xWheelPos, yWheelPos,
        groundTruthState.pose.Rotation() +
            wpi::math::Rotation2d{distribution(generator) * 0.001_rad});

    odometryDistanceTravelled +=
        lastPose.Translation().Distance(xhat.Translation());
    double error = groundTruthState.pose.Translation()
                       .Distance(xhat.Translation())
                       .value();

    if (error > maxError) {
      maxError = error;
    }
    errorSum += error;

    t += dt;
  }

  EXPECT_LT(errorSum / (trajectory.Duration().value() / dt.value()), 0.15);
  EXPECT_LT(maxError, 0.3);
  EXPECT_NEAR(trajectoryDistanceTravelled.value(),
              odometryDistanceTravelled.value(),
              trajectoryDistanceTravelled.value() * 0.05);
}

TEST_F(TwoDeadWheelOdometryTest, AccuracyFacingXAxis) {
  auto xWheelPos = 0_m;
  auto yWheelPos = 0_m;

  wpi::math::DrivetrainSplineTrajectory trajectory =
      wpi::math::DrivetrainSplineTrajectoryGenerator::Generate(
          std::vector{wpi::math::Pose2d{0_m, 0_m, 45_deg},
                      wpi::math::Pose2d{20_m, 20_m, -90_deg},
                      wpi::math::Pose2d{10_m, 10_m, 135_deg},
                      wpi::math::Pose2d{30_m, 30_m, -90_deg},
                      wpi::math::Pose2d{20_m, 20_m, 45_deg}},
          wpi::math::TrajectoryConfig(0.5_mps, 2.0_mps_sq));

  odometry.ResetPosition(xWheelPos, yWheelPos, Rotation2d{}, Pose2d{});

  std::default_random_engine generator;
  std::normal_distribution<double> distribution(0.0, 1.0);

  wpi::units::second_t dt = 20_ms;
  wpi::units::second_t t = 0_s;

  double maxError = -std::numeric_limits<double>::max();
  double errorSum = 0;

  wpi::units::meter_t odometryDistanceTravelled = 0_m;
  wpi::units::meter_t trajectoryDistanceTravelled = 0_m;

  while (t < trajectory.Duration()) {
    wpi::math::DrivetrainSplineSample groundTruthState = trajectory.SampleAt(t);

    trajectoryDistanceTravelled +=
        groundTruthState.ForwardVelocity() * dt +
        0.5 * groundTruthState.ForwardAcceleration() * dt * dt;

    Eigen::Vector2d wheelVelocities =
        m_inverseKinematicsMatrix *
        Eigen::Vector3d{groundTruthState.ForwardVelocity().value() *
                            groundTruthState.pose.Rotation().Cos(),
                        groundTruthState.ForwardVelocity().value() *
                            groundTruthState.pose.Rotation().Sin(),
                        0};

    auto xWheelVel = wpi::units::meters_per_second_t{wheelVelocities(0, 0)} +
                     distribution(generator) * 0.05_mps;
    auto yWheelVel = wpi::units::meters_per_second_t{wheelVelocities(1, 0)} +
                     distribution(generator) * 0.05_mps;

    xWheelPos += xWheelVel * dt;
    yWheelPos += yWheelVel * dt;

    auto lastPose = odometry.GetPose();

    auto xhat = odometry.Update(
        xWheelPos, yWheelPos,
        wpi::math::Rotation2d{distribution(generator) * 0.001_rad});

    odometryDistanceTravelled +=
        lastPose.Translation().Distance(xhat.Translation());
    double error = groundTruthState.pose.Translation()
                       .Distance(xhat.Translation())
                       .value();

    if (error > maxError) {
      maxError = error;
    }
    errorSum += error;

    t += dt;
  }

  EXPECT_LT(errorSum / (trajectory.Duration().value() / dt.value()), 0.15);
  EXPECT_LT(maxError, 0.3);
  EXPECT_NEAR(trajectoryDistanceTravelled.value(),
              odometryDistanceTravelled.value(),
              trajectoryDistanceTravelled.value() * 0.05);
}
