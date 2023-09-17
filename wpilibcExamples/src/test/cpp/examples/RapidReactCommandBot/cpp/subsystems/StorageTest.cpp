// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc/simulation/DriverStationSim.h>
#include <frc2/command/CommandScheduler.h>
#include <gtest/gtest.h>

#include "sim/StorageSim.h"
#include "subsystems/Storage.h"

class StorageTest : public testing::Test {
 public:
  void TearDown() override {
    frc2::CommandScheduler::GetInstance().CancelAll();
  }

 protected:
  Storage storage;  // real subsystem
  StorageSim sim;   // simulation controller
};

TEST_F(StorageTest, StorageCommand) {
  frc2::CommandPtr commandHolder = storage.RunCommand();

  frc::sim::DriverStationSim::SetEnabled(true);
  frc::sim::DriverStationSim::NotifyNewData();

  commandHolder.Schedule();

  frc2::CommandScheduler::GetInstance().Run();
  sim.SimulationPeriodic();
  frc2::CommandScheduler::GetInstance().Run();
  sim.SimulationPeriodic();

  EXPECT_NEAR(StorageConstants::kStorageDutyCycle, sim.GetMotor(), 1e-6);
  EXPECT_TRUE(commandHolder.IsScheduled());
}

TEST_F(StorageTest, IdleCommand) {
  frc::sim::DriverStationSim::SetEnabled(true);
  frc::sim::DriverStationSim::NotifyNewData();

  for (size_t i = 0; i < 150; i++) {
    frc2::CommandScheduler::GetInstance().Run();
    sim.SimulationPeriodic();
  }

  EXPECT_NEAR(0.0, sim.GetMotor(), 1e-6);
  auto idleCommand = storage.GetCurrentCommand();
  EXPECT_STREQ("Idle", idleCommand->GetName());
}
