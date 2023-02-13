// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc/simulation/DriverStationSim.h>
#include <frc2/command/CommandScheduler.h>
#include <gtest/gtest.h>

#include "sim/ShooterSim.h"
#include "subsystems/Shooter.h"

class ShooterTest : public testing::Test {
 public:
  void TearDown() override {
    frc2::CommandScheduler::GetInstance().CancelAll();
    frc2::CommandScheduler::GetInstance().GetDefaultButtonLoop()->Clear();
  }

 protected:
  Shooter shooter;  // real subsystem
  ShooterSim sim;   // simulation controller
};

TEST_F(ShooterTest, IdleCommand) {
  // Check default command registered properly
  frc2::Command* defaultCommand = shooter.GetDefaultCommand();
  EXPECT_TRUE(defaultCommand);
  EXPECT_STREQ("Idle", defaultCommand->GetName().c_str());
  EXPECT_TRUE(defaultCommand->RunsWhenDisabled());

  // Check default command functionality
  frc2::CommandScheduler::GetInstance().Run();
  frc2::CommandScheduler::GetInstance().Run();
  frc2::CommandScheduler::GetInstance().Run();
  auto command = shooter.GetCurrentCommand();
  EXPECT_TRUE(command);
  EXPECT_STREQ("Idle", command->GetName().c_str());
  EXPECT_DOUBLE_EQ(0.0, sim.GetShooterMotor());
  EXPECT_DOUBLE_EQ(0.0, sim.GetFeederMotor());
}

TEST_F(ShooterTest, ShootCommand) {
  frc2::CommandPtr commandHolder =
      shooter.ShootCommand(ShooterConstants::kShooterTarget);
  frc2::Command* command = commandHolder.get();
  EXPECT_STREQ("Shoot", command->GetName().c_str());

  frc::sim::DriverStationSim::SetEnabled(true);
  frc::sim::DriverStationSim::NotifyNewData();

  commandHolder.Schedule();

  EXPECT_DOUBLE_EQ(0.0, sim.GetFeederMotor());

  for (size_t i = 0; i < 350; i++) {
    frc2::CommandScheduler::GetInstance().Run();
    sim.SimulationPeriodic();
    EXPECT_NE(0.0, sim.GetShooterMotor());
  }
  EXPECT_NEAR(ShooterConstants::kShooterTarget.value(),
              shooter.GetShooterVelocity().value(),
              ShooterConstants::kShooterTolerance.value());
  EXPECT_DOUBLE_EQ(ShooterConstants::kFeederSpeed, sim.GetFeederMotor());
}
