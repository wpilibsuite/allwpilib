// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc/simulation/DriverStationSim.h>
#include <frc2/command/CommandScheduler.h>
#include <gtest/gtest.h>

#include "sim/IntakeSim.h"
#include "subsystems/Intake.h"

class IntakeTest : public testing::Test {
 public:
  void TearDown() override {
    frc2::CommandScheduler::GetInstance().CancelAll();
  }

 protected:
  Intake intake;   // real subsystem
  IntakeSim sim;  // simulation controller
};

TEST_F(IntakeTest, IntakeCommand) {
  frc2::CommandPtr commandHolder = intake.IntakeCommand();

  frc::sim::DriverStationSim::SetEnabled(true);
  frc::sim::DriverStationSim::NotifyNewData();

  commandHolder.Schedule();

  frc2::CommandScheduler::GetInstance().Run();
  sim.SimulationPeriodic();

  EXPECT_NEAR(IntakeConstants::kIntakeDutyCycle, sim.GetMotor(), 1e-6);
  EXPECT_TRUE(sim.IsDeployed());
}

TEST_F(IntakeTest, RetractCommand) {
  frc2::CommandPtr commandHolder = intake.RetractCommand();

  frc::sim::DriverStationSim::SetEnabled(true);
  frc::sim::DriverStationSim::NotifyNewData();

  commandHolder.Schedule();

  frc2::CommandScheduler::GetInstance().Run();
  sim.SimulationPeriodic();

  EXPECT_NEAR(0.0, sim.GetMotor(), 1e-6);
  EXPECT_FALSE(sim.IsDeployed());
}
