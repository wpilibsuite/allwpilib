// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <string>
#include <thread>

#include <gtest/gtest.h>

#include "Constants.hpp"
#include "Robot.hpp"
#include "wpi/hal/DriverStationTypes.h"
#include "wpi/hal/simulation/MockHooks.h"
#include "wpi/simulation/DriverStationSim.hpp"
#include "wpi/simulation/JoystickSim.hpp"
#include "wpi/simulation/PWMMotorControllerSim.hpp"
#include "wpi/simulation/SimHooks.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/mass.hpp"
#include "wpi/units/time.hpp"

using namespace Constants;

class ElevatorSimulationTest : public testing::Test {
  Robot m_robot;
  std::optional<std::thread> m_thread;

 protected:
  wpi::sim::PWMMotorControllerSim m_motorSim{Constants::kMotorPort};
  wpi::sim::EncoderSim m_encoderSim =
      wpi::sim::EncoderSim::CreateForChannel(Constants::kEncoderAChannel);
  wpi::sim::JoystickSim m_joystickSim{Constants::kJoystickPort};

 public:
  void SetUp() override {
    wpi::sim::PauseTiming();
    wpi::sim::SetProgramStarted(false);

    m_thread = std::thread([&] { m_robot.StartCompetition(); });
    wpi::sim::WaitForProgramStart();
  }

  void TearDown() override {
    m_robot.EndCompetition();
    m_thread->join();

    m_encoderSim.ResetData();
    wpi::sim::DriverStationSim::ResetData();
  }
};

TEST_F(ElevatorSimulationTest, Teleop) {
  // teleop init
  {
    wpi::sim::DriverStationSim::SetRobotMode(HAL_ROBOTMODE_TELEOPERATED);
    wpi::sim::DriverStationSim::SetEnabled(true);
    wpi::sim::DriverStationSim::NotifyNewData();

    EXPECT_TRUE(m_encoderSim.GetInitialized());
  }

  {
    // advance 50 timesteps
    wpi::sim::StepTiming(1_s);

    // Ensure elevator is still at 0.
    EXPECT_NEAR(0.0, m_encoderSim.GetDistance(), 0.05);
  }

  {
    // Press button to reach setpoint
    m_joystickSim.SetTrigger(true);
    m_joystickSim.NotifyNewData();

    // advance 75 timesteps
    wpi::sim::StepTiming(1.5_s);

    EXPECT_NEAR(kSetpoint.value(), m_encoderSim.GetDistance(), 0.05);

    // advance 25 timesteps to see setpoint is held.
    wpi::sim::StepTiming(0.5_s);

    EXPECT_NEAR(kSetpoint.value(), m_encoderSim.GetDistance(), 0.05);
  }

  {
    // Unpress the button to go back down
    m_joystickSim.SetTrigger(false);
    m_joystickSim.NotifyNewData();

    // advance 75 timesteps
    wpi::sim::StepTiming(1.5_s);

    EXPECT_NEAR(0.0, m_encoderSim.GetDistance(), 0.05);
  }

  {
    // Press button to go back up
    m_joystickSim.SetTrigger(true);
    m_joystickSim.NotifyNewData();

    // advance 75 timesteps
    wpi::sim::StepTiming(1.5_s);

    EXPECT_NEAR(kSetpoint.value(), m_encoderSim.GetDistance(), 0.05);

    // advance 25 timesteps to see setpoint is held.
    wpi::sim::StepTiming(0.5_s);

    EXPECT_NEAR(kSetpoint.value(), m_encoderSim.GetDistance(), 0.05);
  }

  {
    // Disable
    wpi::sim::DriverStationSim::SetEnabled(false);
    wpi::sim::DriverStationSim::NotifyNewData();

    // advance 75 timesteps
    wpi::sim::StepTiming(1.5_s);

    ASSERT_NEAR(0.0, m_motorSim.GetSpeed(), 0.05);
    ASSERT_NEAR(0.0, m_encoderSim.GetDistance(), 0.05);
  }
}
