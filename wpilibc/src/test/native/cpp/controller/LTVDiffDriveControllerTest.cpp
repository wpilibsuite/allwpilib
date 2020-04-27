/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <limits>
#include <random>

#include <Eigen/Core>
#include <units/time.h>
#include <wpi/MathExtras.h>

#include "frc/StateSpaceUtil.h"
#include "frc/controller/ControlAffinePlantInversionFeedforward.h"
#include "frc/controller/LTVDiffDriveController.h"
#include "frc/estimator/DifferentialDriveStateEstimator.h"
#include "frc/geometry/Pose2d.h"
#include "frc/geometry/Rotation2d.h"
#include "frc/kinematics/DifferentialDriveKinematics.h"
#include "frc/kinematics/DifferentialDriveOdometry.h"
#include "frc/system/LinearSystem.h"
#include "frc/system/RungeKutta.h"
#include "frc/system/plant/LinearSystemId.h"
#include "frc/trajectory/TrajectoryGenerator.h"
#include "frc/trajectory/constraint/DifferentialDriveVelocitySystemConstraint.h"
#include "frc2/Timer.h"
#include "gtest/gtest.h"

using namespace frc;

void ScaleCapU(Eigen::Matrix<double, 2, 1>* u) {
  bool outputCapped =
      std::abs((*u)(0, 0)) > 12.0 || std::abs((*u)(1, 0)) > 12.0;

  if (outputCapped) {
    *u *= 12.0 / u->lpNorm<Eigen::Infinity>();
  }
}

TEST(LTVDiffDriveControllerTest, TrackingTest) {
  constexpr auto kDt = 0.02_s;

  LinearSystem<2, 2, 2> plant = frc::LinearSystemId::IdentifyDrivetrainSystem(
      3.02, 0.642, 1.382, 0.08495);

  const DifferentialDriveKinematics kinematics{1_m};

  LTVDiffDriveController controller{
      plant, {0.0625, 0.125, 2.5, 0.95, 0.95}, {12.0, 12.0}, kinematics, kDt};

  std::function<Eigen::Matrix<double, 10, 1>(
      const Eigen::Matrix<double, 10, 1>&, const Eigen::Matrix<double, 2, 1>&)>
      modelDynamics =
          [&](auto& x, auto& u) { return controller.Dynamics(x, u); };

  ControlAffinePlantInversionFeedforward<10, 2> feedforward{modelDynamics, kDt};

  frc::DifferentialDriveStateEstimator estimator{
      plant,
      Eigen::Matrix<double, 10, 1>::Zero(),
      frc::MakeMatrix<10, 1>(0.002, 0.002, 0.0001, 1.5, 1.5, 0.5, 0.5, 10.0,
                             10.0, 2.0),
      frc::MakeMatrix<3, 1>(0.0001, 0.005, 0.005),
      frc::MakeMatrix<3, 1>(0.5, 0.5, 0.5),
      kinematics,
      kDt};

  auto waypoints = std::vector{frc::Pose2d{0_m, 0_m, 0_rad},
                               frc::Pose2d{4.8768_m, 2.7432_m, 0_rad}};

  auto config =
      TrajectoryConfig(12_mps / 3.02, (12_mps_sq / 0.642) - 16.5_mps_sq);
  config.AddConstraint(
      DifferentialDriveVelocitySystemConstraint(plant, kinematics, 8_V));

  auto trajectory =
      frc::TrajectoryGenerator::GenerateTrajectory(waypoints, config);

  controller.Reset();
  estimator.Reset();
  controller.SetTolerance(frc::Pose2d{0.06_m, 0.06_m, 0.06_rad}, 0.3_mps);

  auto t = 0.0_s;
  auto totalTime = trajectory.TotalTime();

  Eigen::Matrix<double, 10, 1> trueXhat;
  trueXhat.setZero();

  Eigen::Matrix<double, 2, 1> u;
  u.setZero();

  Eigen::Matrix<double, 5, 1> prevStateRef;
  prevStateRef.setZero();

  std::cout << "xhat x, xhat y, xhat heading, traj x, traj y, traj heading\n";

  while (t <= totalTime) {
    Eigen::Matrix<double, 3, 1> y = estimator.LocalMeasurementModel(
        trueXhat, Eigen::Matrix<double, 2, 1>::Zero());

    Eigen::Matrix<double, 10, 1> currentState = estimator.UpdateWithTime(
        units::radian_t(y(0, 0)), units::meter_t(y(1, 0)),
        units::meter_t(y(2, 0)), u, t);

    auto desiredState = trajectory.Sample(t);

    const ChassisSpeeds chassisSpeeds{
        desiredState.velocity, 0_mps,
        desiredState.velocity * desiredState.curvature};

    auto [left, right] = kinematics.ToWheelSpeeds(chassisSpeeds);

    Eigen::Matrix<double, 5, 1> stateRef;
    stateRef << desiredState.pose.Translation().X().to<double>(),
        desiredState.pose.Translation().Y().to<double>(),
        desiredState.pose.Rotation().Radians().to<double>(), left.to<double>(),
        right.to<double>();

    prevStateRef = stateRef;

    Eigen::Matrix<double, 10, 1> augmentedRef;
    augmentedRef.block<5, 1>(0, 0) = stateRef;

    u = controller.Calculate(currentState.block<5, 1>(0, 0), desiredState) +
        feedforward.Calculate(augmentedRef);

    ScaleCapU(&u);

    t += kDt;

    trueXhat = frc::RungeKutta(modelDynamics, trueXhat, u, kDt);
    EXPECT_TRUE(controller.AtReference());
  }
}

TEST(LTVDiffDriveControllerTest, TrackingTestNoise) {
  constexpr auto kDt = 0.02_s;

  LinearSystem<2, 2, 2> plant = frc::LinearSystemId::IdentifyDrivetrainSystem(
      3.02, 0.642, 1.382, 0.08495);

  const DifferentialDriveKinematics kinematics{1_m};

  LTVDiffDriveController controller{
      plant, {0.0625, 0.125, 2.5, 0.95, 0.95}, {12.0, 12.0}, kinematics, kDt};

  std::function<Eigen::Matrix<double, 10, 1>(
      const Eigen::Matrix<double, 10, 1>&, const Eigen::Matrix<double, 2, 1>&)>
      controllerDynamics =
          [&](auto& x, auto& u) { return controller.Dynamics(x, u); };

  ControlAffinePlantInversionFeedforward<10, 2> feedforward{controllerDynamics,
                                                            kDt};

  frc::DifferentialDriveStateEstimator estimator{
      plant,
      Eigen::Matrix<double, 10, 1>::Zero(),
      frc::MakeMatrix<10, 1>(0.002, 0.002, 0.0001, 1.5, 1.5, 0.5, 0.5, 10.0,
                             10.0, 2.0),
      frc::MakeMatrix<3, 1>(0.0001, 0.005, 0.005),
      frc::MakeMatrix<3, 1>(0.1, 0.1, 0.01),
      kinematics,
      kDt};

  auto waypoints = std::vector{frc::Pose2d{0_m, 0_m, 0_rad},
                               frc::Pose2d{4.8768_m, 2.7432_m, 0_rad}};

  auto config = TrajectoryConfig(12_mps / 3.02, 12_mps_sq / 0.642);
  config.AddConstraint(
      DifferentialDriveVelocitySystemConstraint(plant, kinematics, 8_V));

  auto trajectory =
      frc::TrajectoryGenerator::GenerateTrajectory(waypoints, config);

  controller.Reset();
  estimator.Reset();
  controller.SetTolerance(frc::Pose2d{0.06_m, 0.06_m, 0.1_rad}, 0.5_mps);

  auto t = 0.0_s;
  auto totalTime = trajectory.TotalTime();

  Eigen::Matrix<double, 10, 1> trueXhat;
  trueXhat.setZero();

  Eigen::Matrix<double, 2, 1> u;
  u.setZero();

  Eigen::Matrix<double, 5, 1> prevStateRef;
  prevStateRef.setZero();

  while (t <= totalTime) {
    Eigen::Matrix<double, 3, 1> y = estimator.LocalMeasurementModel(
        trueXhat, Eigen::Matrix<double, 2, 1>::Zero());

    y += frc::MakeWhiteNoiseVector(0.0001, 0.005, 0.005);

    Eigen::Matrix<double, 10, 1> currentState = estimator.UpdateWithTime(
        units::radian_t(y(0, 0)), units::meter_t(y(1, 0)),
        units::meter_t(y(2, 0)), u, t);

    auto desiredState = trajectory.Sample(t);

    const ChassisSpeeds chassisSpeeds{
        desiredState.velocity, 0_mps,
        desiredState.velocity * desiredState.curvature};

    auto [left, right] = kinematics.ToWheelSpeeds(chassisSpeeds);

    Eigen::Matrix<double, 5, 1> stateRef;
    stateRef << desiredState.pose.Translation().X().to<double>(),
        desiredState.pose.Translation().Y().to<double>(),
        desiredState.pose.Rotation().Radians().to<double>(), left.to<double>(),
        right.to<double>();

    prevStateRef = stateRef;

    Eigen::Matrix<double, 10, 1> augmentedRef;
    augmentedRef.block<5, 1>(0, 0) = stateRef;

    u = controller.Calculate(currentState.block<5, 1>(0, 0), desiredState) +
        feedforward.Calculate(augmentedRef);

    ScaleCapU(&u);

    t += kDt;

    trueXhat = frc::RungeKutta(controllerDynamics, trueXhat, u, kDt);
    EXPECT_TRUE(controller.AtReference());
  }
}
