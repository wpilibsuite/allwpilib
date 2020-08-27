/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <iostream>
#include <limits>
#include <random>

#include <Eigen/Core>
#include <wpi/MathExtras.h>

#include "frc/StateSpaceUtil.h"
#include "frc/controller/ControlAffinePlantInversionFeedforward.h"
#include "frc/estimator/DifferentialDriveStateEstimator.h"
#include "frc/geometry/Pose2d.h"
#include "frc/geometry/Rotation2d.h"
#include "frc/kinematics/DifferentialDriveKinematics.h"
#include "frc/kinematics/DifferentialDriveOdometry.h"
#include "frc/system/LinearSystem.h"
#include "frc/system/plant/LinearSystemId.h"
#include "frc/trajectory/TrajectoryGenerator.h"
#include "frc2/Timer.h"
#include "gtest/gtest.h"

using namespace frc;

void ScaleCappedU(Eigen::Matrix<double, 2, 1>* u) {
  bool outputCapped =
      std::abs((*u)(0, 0)) > 12.0 || std::abs((*u)(1, 0)) > 12.0;

  if (outputCapped) {
    *u *= 12.0 / u->lpNorm<Eigen::Infinity>();
  }
}

TEST(DifferentialDriveStateEstimatorTest, TestAccuracy) {
  constexpr auto dt = 0.00505_s;

  const LinearSystem<2, 2, 2> plant =
      frc::LinearSystemId::IdentifyDrivetrainSystem(3.02, 0.642, 1.382,
                                                    0.08495);

  const DifferentialDriveKinematics kinematics{0.990405073902434_m};

  frc::DifferentialDriveStateEstimator estimator{
      plant,
      Eigen::Matrix<double, 10, 1>::Zero(),
      frc::MakeMatrix<10, 1>(0.002, 0.002, 0.0001, 1.5, 1.5, 0.5, 0.5, 10.0,
                             10.0, 2.0),
      frc::MakeMatrix<3, 1>(0.0001, 0.005, 0.005),
      frc::MakeMatrix<3, 1>(0.5, 0.5, 0.5),
      kinematics,
      dt};

  std::function<Eigen::Matrix<double, 10, 1>(
      const Eigen::Matrix<double, 10, 1>&, const Eigen::Matrix<double, 2, 1>&)>
      modelDynamics =
          [&](auto& x, auto& u) { return estimator.Dynamics(x, u); };

  ControlAffinePlantInversionFeedforward<10, 2> feedforward{modelDynamics, dt};

  auto config = TrajectoryConfig(
      units::meters_per_second_t(12 / 3.5),
      units::meters_per_second_squared_t(12 / 0.642) - 17.5_mps_sq);

  frc::Trajectory trajectory = frc::TrajectoryGenerator::GenerateTrajectory(
      frc::Pose2d(0_m, 0_m, 0_rad), {}, frc::Pose2d(4.8768_m, 2.7432_m, 0_rad),
      config);

  std::default_random_engine generator;
  std::normal_distribution<double> distribution(0.0, 1.0);

  units::second_t t = 0.0_s;

  units::second_t kVisionUpdateRate = 0.1_s;
  frc::Pose2d lastVisionPose;
  units::second_t lastVisionUpdateRealTimestamp{
      -std::numeric_limits<double>::max()};
  units::second_t lastVisionUpdateTime{-std::numeric_limits<double>::max()};
  units::meter_t leftDistance = 0_m;
  units::meter_t rightDistance = 0_m;

  double maxError = -std::numeric_limits<double>::max();
  double errorSum = 0;

  Eigen::Matrix<double, 2, 1> input;

  while (t <= trajectory.TotalTime()) {
    auto groundTruthState = trajectory.Sample(t);

    auto wheelSpeeds = kinematics.ToWheelSpeeds(
        {groundTruthState.velocity, 0_mps,
         groundTruthState.velocity * groundTruthState.curvature});

    Eigen::Matrix<double, 10, 1> x;
    x << groundTruthState.pose.Translation().X().to<double>(),
        groundTruthState.pose.Translation().Y().to<double>(),
        groundTruthState.pose.Rotation().Radians().to<double>(),
        wheelSpeeds.left.to<double>(), wheelSpeeds.right.to<double>(), 0.0, 0.0,
        0.0, 0.0, 0.0;

    input = feedforward.Calculate(x);

    ScaleCappedU(&input);

    if (lastVisionUpdateTime + kVisionUpdateRate < t) {
      if (lastVisionPose != frc::Pose2d()) {
        estimator.ApplyPastGlobalMeasurement(lastVisionPose,
                                             lastVisionUpdateTime);
      }
      lastVisionPose =
          groundTruthState.pose +
          frc::Transform2d(
              frc::Translation2d(distribution(generator) * 0.5 * 1_m,
                                 distribution(generator) * 0.5 * 1_m),
              frc::Rotation2d(distribution(generator) * 0.5 * 1_rad));

      lastVisionUpdateRealTimestamp = frc2::Timer::GetFPGATimestamp();
      lastVisionUpdateTime = t;
    }

    leftDistance += wheelSpeeds.left * dt;
    rightDistance += wheelSpeeds.right * dt;

    leftDistance += distribution(generator) * 0.005_m;
    rightDistance += distribution(generator) * 0.005_m;

    auto xhat = estimator.UpdateWithTime(
        (groundTruthState.pose.Rotation() +
         frc::Rotation2d(units::radian_t(distribution(generator) * 0.0001)))
            .Radians(),
        leftDistance, rightDistance, input, t);

    frc::Translation2d estimatedTranslation{units::meter_t(xhat(0, 0)),
                                            units::meter_t(xhat(1, 0))};

    double error = groundTruthState.pose.Translation()
                       .Distance(estimatedTranslation)
                       .to<double>();
    if (error > maxError) {
      maxError = error;
    }
    errorSum += error;

    std::cout << groundTruthState.pose.Translation().X().to<double>() 
      << "," << groundTruthState.pose.Translation().Y().to<double>() 
      << "," << estimatedTranslation.X().to<double>() << ","
      << estimatedTranslation.Y().to<double>() << "," << error << "\n";

    t += dt;
  }

  EXPECT_LT(errorSum / (trajectory.TotalTime().to<double>() / dt.to<double>()),
            0.2);
  EXPECT_LT(maxError, 0.4);

  std::cout << "Sum over time: " << errorSum / (trajectory.TotalTime().to<double>() / dt.to<double>()) << " max error " << maxError << "\n"; 
}
