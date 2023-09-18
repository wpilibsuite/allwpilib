// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc/simulation/DriverStationSim.h>
#include <frc2/command/CommandScheduler.h>
#include <gtest/gtest.h>

#include "sim/DriveSim.h"
#include "subsystems/Drive.h"

class DriveTest : public testing::Test {
 public:
  void TearDown() override {
    frc2::CommandScheduler::GetInstance().CancelAll();
  }

 protected:
  Drive drive;   // real subsystem
  DriveSim sim;  // simulation controller
};

TEST_F(DriveTest, DriveDistance) {
  frc2::CommandPtr commandHolder = drive.DriveDistanceCommand(
      AutoConstants::kDriveDistance, AutoConstants::kDriveSpeed);

  frc::sim::DriverStationSim::SetEnabled(true);
  frc::sim::DriverStationSim::NotifyNewData();

  commandHolder.Schedule();

  for (size_t i = 0; i < 150; i++) {
    frc2::CommandScheduler::GetInstance().Run();
    sim.SimulationPeriodic();
    EXPECT_NEAR(AutoConstants::kDriveSpeed, sim.GetLeftDutyCycle(), 1e-6);
    EXPECT_NEAR(AutoConstants::kDriveSpeed, sim.GetRightDutyCycle(), 1e-6);
  }
  EXPECT_NEAR(AutoConstants::kDriveDistance.value(),
              drive.GetLeftEncoder().value(), 0.2);
  EXPECT_NEAR(AutoConstants::kDriveDistance.value(),
              drive.GetRightEncoder().value(), 0.2);
}
