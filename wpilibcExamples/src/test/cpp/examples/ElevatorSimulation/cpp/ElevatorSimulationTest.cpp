// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <string>
#include <thread>

#include <frc/simulation/DriverStationSim.h>
#include <frc/simulation/JoystickSim.h>
#include <frc/simulation/PWMMotorControllerSim.h>
#include <frc/simulation/SimHooks.h>
#include <gtest/gtest.h>
#include <hal/simulation/MockHooks.h>
#include <units/length.hpp>
#include <units/mass.hpp>
#include <units/time.hpp>

#include "Constants.h"
#include "Robot.h"

using namespace Constants;

class ElevatorSimulationTest : public testing::Test {
  Robot m_robot;
  std::optional<std::thread> m_thread;

 protected:
  frc::sim::PWMMotorControllerSim m_motorSim{Constants::kMotorPort};
  frc::sim::EncoderSim m_encoderSim =
      frc::sim::EncoderSim::CreateForChannel(Constants::kEncoderAChannel);
  frc::sim::JoystickSim m_joystickSim{Constants::kJoystickPort};

 public:
  void SetUp() override {
    frc::sim::PauseTiming();

    m_thread = std::thread([&] { m_robot.StartCompetition(); });
    frc::sim::StepTiming(0.0_ms);  // Wait for Notifiers
  }

  void TearDown() override {
    m_robot.EndCompetition();
    m_thread->join();

    m_encoderSim.ResetData();
    frc::sim::DriverStationSim::ResetData();
  }
};

TEST_F(ElevatorSimulationTest, Teleop) {
  // teleop init
  {
    frc::sim::DriverStationSim::SetAutonomous(false);
    frc::sim::DriverStationSim::SetEnabled(true);
    frc::sim::DriverStationSim::NotifyNewData();

    EXPECT_TRUE(m_encoderSim.GetInitialized());
  }

  {
    // advance 50 timesteps
    frc::sim::StepTiming(1_s);

    // Ensure elevator is still at 0.
    EXPECT_NEAR(0.0, m_encoderSim.GetDistance(), 0.05);
  }

  {
    // Press button to reach setpoint
    m_joystickSim.SetTrigger(true);
    m_joystickSim.NotifyNewData();

    // advance 75 timesteps
    frc::sim::StepTiming(1.5_s);

    EXPECT_NEAR(kSetpoint.value(), m_encoderSim.GetDistance(), 0.05);

    // advance 25 timesteps to see setpoint is held.
    frc::sim::StepTiming(0.5_s);

    EXPECT_NEAR(kSetpoint.value(), m_encoderSim.GetDistance(), 0.05);
  }

  {
    // Unpress the button to go back down
    m_joystickSim.SetTrigger(false);
    m_joystickSim.NotifyNewData();

    // advance 75 timesteps
    frc::sim::StepTiming(1.5_s);

    EXPECT_NEAR(0.0, m_encoderSim.GetDistance(), 0.05);
  }

  {
    // Press button to go back up
    m_joystickSim.SetTrigger(true);
    m_joystickSim.NotifyNewData();

    // advance 75 timesteps
    frc::sim::StepTiming(1.5_s);

    EXPECT_NEAR(kSetpoint.value(), m_encoderSim.GetDistance(), 0.05);

    // advance 25 timesteps to see setpoint is held.
    frc::sim::StepTiming(0.5_s);

    EXPECT_NEAR(kSetpoint.value(), m_encoderSim.GetDistance(), 0.05);
  }

  {
    // Disable
    frc::sim::DriverStationSim::SetAutonomous(false);
    frc::sim::DriverStationSim::SetEnabled(false);
    frc::sim::DriverStationSim::NotifyNewData();

    // advance 75 timesteps
    frc::sim::StepTiming(1.5_s);

    ASSERT_NEAR(0.0, m_motorSim.GetSpeed(), 0.05);
    ASSERT_NEAR(0.0, m_encoderSim.GetDistance(), 0.05);
  }
}
