// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>
#include <units/current.hpp>
#include <units/math.hpp>
#include <units/moment_of_inertia.hpp>

#include "wpi/math/controller/LTVUnicycleController.hpp"
#include "wpi/math/controller/LinearPlantInversionFeedforward.hpp"
#include "wpi/math/kinematics/DifferentialDriveKinematics.hpp"
#include "frc/simulation/DifferentialDrivetrainSim.h"
#include "wpi/math/system/NumericalIntegration.hpp"
#include "wpi/math/system/plant/DCMotor.hpp"
#include "wpi/math/system/plant/LinearSystemId.hpp"
#include "wpi/math/trajectory/TrajectoryGenerator.hpp"
#include "wpi/math/trajectory/constraint/DifferentialDriveKinematicsConstraint.hpp"

using namespace wpi::math;

TEST(DifferentialDrivetrainSimTest, Convergence) {
  auto motor = DCMotor::NEO(2);
  auto plant = LinearSystemId::DrivetrainVelocitySystem(
      motor, 50_kg, 2_in, 12_in, 0.5_kg_sq_m, 1.0);

  DifferentialDriveKinematics kinematics{24_in};
  sim::DifferentialDrivetrainSim sim{
      plant, 24_in, motor,
      1.0,   2_in,  {0.001, 0.001, 0.0001, 0.1, 0.1, 0.005, 0.005}};

  LinearPlantInversionFeedforward feedforward{plant, 20_ms};
  LTVUnicycleController feedback{20_ms};

  feedforward.Reset(Vectord<2>{0.0, 0.0});

  // Ground truth.
  Vectord<7> groundTruthX = Vectord<7>::Zero();

  TrajectoryConfig config{1_mps, 1_mps_sq};
  config.AddConstraint(
      DifferentialDriveKinematicsConstraint(kinematics, 1_mps));

  auto trajectory = TrajectoryGenerator::GenerateTrajectory(
      Pose2d{}, {}, Pose2d{2_m, 2_m, 0_rad}, config);

  for (auto t = 0_s; t < trajectory.TotalTime(); t += 20_ms) {
    auto state = trajectory.Sample(t);
    auto feedbackOut = feedback.Calculate(sim.GetPose(), state);

    auto [l, r] = kinematics.ToWheelSpeeds(feedbackOut);
    auto voltages =
        feedforward.Calculate(Vectord<2>{l.value(), r.value()});

    // Sim periodic code.
    sim.SetInputs(units::volt_t{voltages(0, 0)}, units::volt_t{voltages(1, 0)});
    sim.Update(20_ms);

    // Update ground truth.
    groundTruthX = RKDP(
        [&sim](const auto& x, const auto& u) -> Vectord<7> {
          return sim.Dynamics(x, u);
        },
        groundTruthX, voltages, 20_ms);
  }

  // 2 inch tolerance is OK since our ground truth is an approximation of the
  // ODE solution using RKDP anyway
  EXPECT_NEAR(groundTruthX(0, 0), sim.GetPose().X().value(), 0.05);
  EXPECT_NEAR(groundTruthX(1, 0), sim.GetPose().Y().value(), 0.05);
  EXPECT_NEAR(groundTruthX(2, 0), sim.GetHeading().Radians().value(), 0.01);
}

TEST(DifferentialDrivetrainSimTest, Current) {
  auto motor = DCMotor::NEO(2);
  auto plant = LinearSystemId::DrivetrainVelocitySystem(
      motor, 50_kg, 2_in, 12_in, 0.5_kg_sq_m, 1.0);

  DifferentialDriveKinematics kinematics{24_in};
  sim::DifferentialDrivetrainSim sim{plant, 24_in, motor, 1.0, 2_in};

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

TEST(DifferentialDrivetrainSimTest, ModelStability) {
  auto motor = DCMotor::NEO(2);
  auto plant = LinearSystemId::DrivetrainVelocitySystem(
      motor, 50_kg, 2_in, 12_in, 2_kg_sq_m, 5.0);

  DifferentialDriveKinematics kinematics{24_in};
  sim::DifferentialDrivetrainSim sim{plant, 24_in, motor, 1.0, 2_in};

  sim.SetInputs(2_V, 4_V);

  // 10 seconds should be enough time to verify stability
  for (int i = 0; i < 500; ++i) {
    sim.Update(20_ms);
  }

  EXPECT_LT(units::math::abs(sim.GetPose().Translation().Norm()), 100_m);
}
