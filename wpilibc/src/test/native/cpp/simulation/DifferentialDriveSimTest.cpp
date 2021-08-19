// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <units/current.h>
#include <units/math.h>
#include <units/moment_of_inertia.h>

#include "frc/controller/LinearPlantInversionFeedforward.h"
#include "frc/controller/RamseteController.h"
#include "frc/kinematics/DifferentialDriveKinematics.h"
#include "frc/simulation/DifferentialDrivetrainSim.h"
#include "frc/system/NumericalIntegration.h"
#include "frc/system/plant/DCMotor.h"
#include "frc/system/plant/LinearSystemId.h"
#include "frc/trajectory/TrajectoryGenerator.h"
#include "frc/trajectory/constraint/DifferentialDriveKinematicsConstraint.h"
#include "gtest/gtest.h"

TEST(DifferentialDriveSim, Convergence) {
  auto motor = frc::DCMotor::NEO(2);
  auto plant = frc::LinearSystemId::DrivetrainVelocitySystem(
      motor, 50_kg, 2_in, 12_in, 0.5_kg_sq_m, 1.0);

  frc::DifferentialDriveKinematics kinematics{24_in};
  frc::sim::DifferentialDrivetrainSim sim{
      plant, 24_in, motor,
      1.0,   2_in,  {0.001, 0.001, 0.0001, 0.1, 0.1, 0.005, 0.005}};

  frc::LinearPlantInversionFeedforward feedforward{plant, 20_ms};
  frc::RamseteController ramsete;

  feedforward.Reset(Eigen::Vector<double, 2>{0.0, 0.0});

  // Ground truth.
  Eigen::Vector<double, 7> groundTruthX = Eigen::Vector<double, 7>::Zero();

  frc::TrajectoryConfig config{1_mps, 1_mps_sq};
  config.AddConstraint(
      frc::DifferentialDriveKinematicsConstraint(kinematics, 1_mps));

  auto trajectory = frc::TrajectoryGenerator::GenerateTrajectory(
      frc::Pose2d(), {}, frc::Pose2d(2_m, 2_m, 0_rad), config);

  // NOLINTNEXTLINE
  for (double t = 0; t < trajectory.TotalTime().to<double>(); t += 0.02) {
    auto state = trajectory.Sample(20_ms);
    auto ramseteOut = ramsete.Calculate(sim.GetPose(), state);

    auto [l, r] = kinematics.ToWheelSpeeds(ramseteOut);
    auto voltages = feedforward.Calculate(
        Eigen::Vector<double, 2>{l.to<double>(), r.to<double>()});

    // Sim periodic code.
    sim.SetInputs(units::volt_t(voltages(0, 0)), units::volt_t(voltages(1, 0)));
    sim.Update(20_ms);

    // Update ground truth.
    groundTruthX = frc::RK4(
        [&sim](const auto& x, const auto& u) -> Eigen::Vector<double, 7> {
          return sim.Dynamics(x, u);
        },
        groundTruthX, voltages, 20_ms);
  }

  // 2 inch tolerance is OK since our ground truth is an approximation of the
  // ODE solution using RK4 anyway
  EXPECT_NEAR(groundTruthX(0, 0), sim.GetPose().X().to<double>(), 0.05);
  EXPECT_NEAR(groundTruthX(1, 0), sim.GetPose().Y().to<double>(), 0.05);
  EXPECT_NEAR(groundTruthX(2, 0), sim.GetHeading().Radians().to<double>(),
              0.01);
}

TEST(DifferentialDriveSim, Current) {
  auto motor = frc::DCMotor::NEO(2);
  auto plant = frc::LinearSystemId::DrivetrainVelocitySystem(
      motor, 50_kg, 2_in, 12_in, 0.5_kg_sq_m, 1.0);

  frc::DifferentialDriveKinematics kinematics{24_in};
  frc::sim::DifferentialDrivetrainSim sim{plant, 24_in, motor, 1.0, 2_in};

  sim.SetInputs(-12_V, 12_V);
  for (int i = 0; i < 10; ++i) {
    sim.Update(20_ms);
  }
  EXPECT_TRUE(sim.GetCurrentDraw() > 0_A);

  sim.SetInputs(12_V, 12_V);
  for (int i = 0; i < 20; ++i) {
    sim.Update(20_ms);
  }
  EXPECT_TRUE(sim.GetCurrentDraw() > 0_A);

  sim.SetInputs(-12_V, 12_V);
  for (int i = 0; i < 30; ++i) {
    sim.Update(20_ms);
  }
  EXPECT_TRUE(sim.GetCurrentDraw() > 0_A);
}

TEST(DifferentialDriveSim, ModelStability) {
  auto motor = frc::DCMotor::NEO(2);
  auto plant = frc::LinearSystemId::DrivetrainVelocitySystem(
      motor, 50_kg, 2_in, 12_in, 2_kg_sq_m, 5.0);

  frc::DifferentialDriveKinematics kinematics{24_in};
  frc::sim::DifferentialDrivetrainSim sim{plant, 24_in, motor, 1.0, 2_in};

  sim.SetInputs(2_V, 4_V);

  // 10 seconds should be enough time to verify stability
  for (int i = 0; i < 500; ++i) {
    sim.Update(20_ms);
  }

  EXPECT_LT(units::math::abs(sim.GetPose().Translation().Norm()), 100_m);
}
