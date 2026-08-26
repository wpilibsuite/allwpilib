// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/TwoDeadWheelOdometry3d.hpp"

#include <limits>
#include <random>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "wpi/math/TestAssertions.hpp"
#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/geometry/Pose3d.hpp"
#include "wpi/math/geometry/Rotation2d.hpp"
#include "wpi/math/geometry/Rotation3d.hpp"
#include "wpi/math/geometry/Translation2d.hpp"
#include "wpi/math/geometry/Translation3d.hpp"
#include "wpi/math/trajectory/DrivetrainSplineSample.hpp"
#include "wpi/math/trajectory/DrivetrainSplineTrajectory.hpp"
#include "wpi/math/trajectory/DrivetrainSplineTrajectoryGenerator.hpp"
#include "wpi/math/trajectory/TrajectoryConfig.hpp"
#include "wpi/units/acceleration.hpp"
#include "wpi/units/angle.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/time.hpp"
#include "wpi/units/velocity.hpp"

using namespace wpi::math;

class TwoDeadWheelOdometry3dTest {
 protected:
  wpi::units::meter_t m_xWheelYPos = 1_m;
  wpi::units::meter_t m_yWheelXPos = 1_m;

  TwoDeadWheelOdometry3d odometry{m_xWheelYPos, m_yWheelXPos, 0_m,
                                  0_m,          Rotation3d{}, Pose3d{}};

  Matrixd<2, 3> m_inverseKinematicsMatrix = Matrixd<2, 3>{
      {1, 0, -m_xWheelYPos.value()}, {0, 1, m_yWheelXPos.value()}};
};

TEST_CASE_METHOD(TwoDeadWheelOdometry3dTest,
                 "TwoDeadWheelOdometry3dTest MultipleConsecutiveUpdates",
                 "[wpimath]") {
  odometry.ResetPosition(1_m, 1_m, Rotation3d{}, Pose3d{});

  odometry.Update(1_m, 1_m, Rotation3d{});
  auto secondPose = odometry.Update(1_m, 1_m, Rotation3d{});

  CHECK_NEAR(secondPose.X().value(), 0.0, 0.01);
  CHECK_NEAR(secondPose.Y().value(), 0.0, 0.01);
  CHECK_NEAR(secondPose.Z().value(), 0.0, 0.01);
  CHECK_NEAR(secondPose.Rotation().X().value(), 0.0, 0.01);
  CHECK_NEAR(secondPose.Rotation().Y().value(), 0.0, 0.01);
  CHECK_NEAR(secondPose.Rotation().Z().value(), 0.0, 0.01);
}

TEST_CASE_METHOD(TwoDeadWheelOdometry3dTest,
                 "TwoDeadWheelOdometry3dTest TwoIterations", "[wpimath]") {
  odometry.ResetPosition(0_m, 0_m, Rotation3d{}, Pose3d{});

  odometry.Update(0_m, 0_m, Rotation3d{});
  auto pose = odometry.Update(0.1_m, 0_m, Rotation3d{});

  CHECK_NEAR(pose.X().value(), 0.1, 0.01);
  CHECK_NEAR(pose.Y().value(), 0.0, 0.01);
  CHECK_NEAR(pose.Rotation().X().value(), 0.0, 0.01);
  CHECK_NEAR(pose.Rotation().Y().value(), 0.0, 0.01);
  CHECK_NEAR(pose.Rotation().Z().value(), 0.0, 0.01);
}

TEST_CASE_METHOD(TwoDeadWheelOdometry3dTest,
                 "TwoDeadWheelOdometry3dTest GyroAngleReset", "[wpimath]") {
  odometry.ResetPosition(0_m, 0_m, Rotation3d{0_rad, 0_rad, 90_deg}, Pose3d{});

  odometry.Update(1_m, 0_m, Rotation3d{0_rad, 0_rad, 90_deg});
  auto pose = odometry.Update(1_m, 0_m, Rotation3d{0_rad, 0_rad, 90_deg});

  CHECK_NEAR(pose.X().value(), 1.0, 0.01);
  CHECK_NEAR(pose.Y().value(), 0.0, 0.01);
  CHECK_NEAR(pose.Z().value(), 0.0, 0.01);
  CHECK_NEAR(pose.Rotation().X().value(), 0.0, 0.01);
  CHECK_NEAR(pose.Rotation().Y().value(), 0.0, 0.01);
  CHECK_NEAR(pose.Rotation().Z().value(), 0.0, 0.01);
}

TEST_CASE_METHOD(TwoDeadWheelOdometry3dTest,
                 "TwoDeadWheelOdometry3dTest AccuracyFacingTrajectory",
                 "[wpimath]") {
  auto xWheelPos = 0_m;
  auto yWheelPos = 0_m;

  wpi::math::DrivetrainSplineTrajectory trajectory =
      wpi::math::DrivetrainSplineTrajectoryGenerator::Generate(
          std::vector{wpi::math::Pose2d{0_m, 0_m, 0_deg},
                      wpi::math::Pose2d{20_m, 20_m, 0_deg},
                      wpi::math::Pose2d{10_m, 10_m, 180_deg},
                      wpi::math::Pose2d{30_m, 30_m, 0_deg},
                      wpi::math::Pose2d{20_m, 20_m, 180_deg},
                      wpi::math::Pose2d{10_m, 10_m, 0_deg}},
          wpi::math::TrajectoryConfig(0.5_mps, 2.0_mps_sq));

  odometry.ResetPosition(xWheelPos, yWheelPos,
                         Rotation3d{trajectory.InitialPose().Rotation()},
                         Pose3d{trajectory.InitialPose()});

  std::mt19937 generator{5190};
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

    auto xWheelVel = wpi::units::meters_per_second_t{wheelVelocities(0)} +
                     distribution(generator) * 0.05_mps;
    auto yWheelVel = wpi::units::meters_per_second_t{wheelVelocities(1)} +
                     distribution(generator) * 0.05_mps;

    xWheelPos += xWheelVel * dt;
    yWheelPos += yWheelVel * dt;

    auto lastPose = odometry.GetPose();

    // Due to the forward kinematics having a dependency on the gyro angle being
    // accurate, if the gyro angle is noisy, the error *very* quickly compounds.
    // The simulated sensor noise in this class's tests are an order of
    // magnitude lower than the other odometry tests for this reason.
    auto xhat = odometry.Update(
        xWheelPos, yWheelPos,
        wpi::math::Rotation3d{
            groundTruthState.pose.Rotation() +
            wpi::math::Rotation2d{distribution(generator) * 0.001_rad}});
    odometryDistanceTravelled +=
        lastPose.Translation().Distance(xhat.Translation());
    double error = groundTruthState.pose.Translation()
                       .Distance(xhat.Translation().ToTranslation2d())
                       .value();

    if (error > maxError) {
      maxError = error;
    }
    errorSum += error;

    t += dt;
  }

  CHECK(errorSum / (trajectory.Duration().value() / dt.value()) < 0.35);
  CHECK(maxError < 0.35);
  CHECK_NEAR(trajectoryDistanceTravelled.value(),
             odometryDistanceTravelled.value(),
             trajectoryDistanceTravelled.value() * 0.05);
}

TEST_CASE_METHOD(TwoDeadWheelOdometry3dTest,
                 "TwoDeadWheelOdometry3dTest AccuracyFacingXAxis",
                 "[wpimath]") {
  auto xWheelPos = 0_m;
  auto yWheelPos = 0_m;

  wpi::math::DrivetrainSplineTrajectory trajectory =
      wpi::math::DrivetrainSplineTrajectoryGenerator::Generate(
          std::vector{wpi::math::Pose2d{0_m, 0_m, 0_deg},
                      wpi::math::Pose2d{20_m, 20_m, 45_deg},
                      wpi::math::Pose2d{10_m, 10_m, -90_deg},
                      wpi::math::Pose2d{30_m, 30_m, 135_deg},
                      wpi::math::Pose2d{20_m, 20_m, -90_deg},
                      wpi::math::Pose2d{10_m, 10_m, 0_deg}},
          wpi::math::TrajectoryConfig(0.5_mps, 2.0_mps_sq));

  odometry.ResetPosition(xWheelPos, yWheelPos, Rotation3d{}, Pose3d{});

  std::mt19937 generator{5190};
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

    // Due to the forward kinematics having a dependency on the gyro angle being
    // accurate, if the gyro angle is noisy, the error *very* quickly compounds.
    // The simulated sensor noise in this class's tests are an order of
    // magnitude lower than the other odometry tests for this reason.
    auto xhat = odometry.Update(
        xWheelPos, yWheelPos,
        wpi::math::Rotation3d{distribution(generator) * 0.001_rad});

    odometryDistanceTravelled +=
        lastPose.Translation().Distance(xhat.Translation());
    double error = groundTruthState.pose.Translation()
                       .Distance(xhat.Translation().ToTranslation2d())
                       .value();

    if (error > maxError) {
      maxError = error;
    }
    errorSum += error;

    t += dt;
  }

  CHECK(errorSum / (trajectory.Duration().value() / dt.value()) < 0.15);
  CHECK(maxError < 0.3);
  CHECK_NEAR(trajectoryDistanceTravelled.value(),
             odometryDistanceTravelled.value(),
             trajectoryDistanceTravelled.value() * 0.05);
}
