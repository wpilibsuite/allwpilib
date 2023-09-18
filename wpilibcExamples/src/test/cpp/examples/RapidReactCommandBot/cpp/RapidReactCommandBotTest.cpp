// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <string>
#include <thread>

#include <frc/simulation/DriverStationSim.h>
#include <frc/simulation/SimHooks.h>
#include <hal/simulation/MockHooks.h>
#include <frc/simulation/XboxControllerSim.h>
#include <gtest/gtest.h>
#include <units/angle.h>
#include <units/length.h>
#include <units/mass.h>
#include <units/time.h>

#include "Robot.h"
#include "sim/DriveSim.h"
#include "sim/IntakeSim.h"
#include "sim/ShooterSim.h"
#include "sim/StorageSim.h"

class RapidReactCommandBotTest : public testing::Test {
 protected:
  Robot m_robot;
  std::optional<std::thread> m_thread;
  DriveSim m_driveSim;
  ShooterSim m_shooterSim;
  StorageSim m_storageSim;
  IntakeSim m_intakeSim;
  frc::sim::XboxControllerSim m_driverController{OIConstants::kDriverControllerPort};

 public:
  void SetUp() override {
    frc::sim::PauseTiming();
    frc::sim::DriverStationSim::ResetData();

    m_thread = std::thread([&] { m_robot.StartCompetition(); });
    frc::sim::StepTiming(0.0_ms);  // Wait for Notifiers
  }

  void TearDown() override {
    m_robot.EndCompetition();
    m_thread->join();
  }
};

TEST_F(RapidReactCommandBotTest, Auto) {
  // auto init
  frc::sim::DriverStationSim::SetAutonomous(true);
  frc::sim::DriverStationSim::SetEnabled(true);
  frc::sim::DriverStationSim::NotifyNewData();

  frc::sim::StepTiming(1_s);

  EXPECT_NEAR(AutoConstants::kDriveSpeed, m_driveSim.GetLeftDutyCycle(), 1e-6);
  EXPECT_NEAR(AutoConstants::kDriveSpeed, m_driveSim.GetRightDutyCycle(), 1e-6);

  // Timeout passed
  frc::sim::StepTiming(2.1_s);

  EXPECT_NEAR(AutoConstants::kDriveDistance.value(),
              m_driveSim.GetRightDistance().value(), 0.2);
  EXPECT_NEAR(AutoConstants::kDriveDistance.value(),
              m_driveSim.GetRightDistance().value(), 0.2);

  EXPECT_NEAR(0.0, m_driveSim.GetLeftDutyCycle(), 1e-6);
  EXPECT_NEAR(0.0, m_driveSim.GetRightDutyCycle(), 1e-6);
}

TEST_F(RapidReactCommandBotTest, StorageFullTriggers) {
  frc::sim::DriverStationSim::SetAutonomous(false);
  frc::sim::DriverStationSim::SetEnabled(true);
  frc::sim::DriverStationSim::NotifyNewData();

  {
    // When empty, storage should run.
    m_storageSim.SetIsFull(false);
    frc::sim::StepTiming(60_ms);
    EXPECT_NEAR(StorageConstants::kStorageDutyCycle, m_storageSim.GetMotor(), 1e-6);
  }

  {
    // Press the 'X' button
    m_driverController.SetXButton(true);
    frc::sim::DriverStationSim::NotifyNewData();
    frc::sim::StepTiming(60_ms);
    m_driverController.SetXButton(false);
    frc::sim::DriverStationSim::NotifyNewData();
    frc::sim::StepTiming(60_ms);
    // Intake should be running
    EXPECT_TRUE(m_intakeSim.IsDeployed());
    EXPECT_NEAR(IntakeConstants::kIntakeDutyCycle, m_intakeSim.GetMotor(), 1e-6);

    // Storage is full
    m_storageSim.SetIsFull(true);

    frc::sim::StepTiming(60_ms);
    // Intake and storage should stop
    EXPECT_NEAR(0.0, m_storageSim.GetMotor(), 1e-6);
    EXPECT_FALSE(m_intakeSim.IsDeployed());
    EXPECT_NEAR(0.0, m_intakeSim.GetMotor(), 1e-6);
  }
}