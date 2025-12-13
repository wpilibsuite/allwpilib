// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <string>
#include <thread>

#include <gtest/gtest.h>

#include "Constants.hpp"
#include "Robot.hpp"
#include "wpi/hal/simulation/MockHooks.h"
#include "wpi/simulation/DriverStationSim.hpp"
#include "wpi/simulation/JoystickSim.hpp"
#include "wpi/simulation/PWMMotorControllerSim.hpp"
#include "wpi/simulation/SimHooks.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/time.hpp"
#include "wpi/util/Preferences.hpp"

class ArmSimulationTest : public testing::TestWithParam<wpi::units::degree_t> {
  Robot m_robot;
  std::optional<std::thread> m_thread;

 protected:
  wpi::sim::PWMMotorControllerSim m_motorSim{kMotorPort};
  wpi::sim::EncoderSim m_encoderSim =
      wpi::sim::EncoderSim::CreateForChannel(kEncoderAChannel);
  wpi::sim::JoystickSim m_joystickSim{kJoystickPort};

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
    wpi::Preferences::RemoveAll();
  }
};

TEST_P(ArmSimulationTest, Teleop) {
  EXPECT_TRUE(wpi::Preferences::ContainsKey(kArmPositionKey));
  EXPECT_TRUE(wpi::Preferences::ContainsKey(kArmPKey));
  wpi::Preferences::SetDouble(kArmPositionKey, GetParam().value());
  wpi::units::degree_t setpoint = GetParam();
  EXPECT_DOUBLE_EQ(setpoint.value(),
                   wpi::Preferences::GetDouble(kArmPositionKey, NAN));

  // teleop init
  {
    wpi::sim::DriverStationSim::SetAutonomous(false);
    wpi::sim::DriverStationSim::SetEnabled(true);
    wpi::sim::DriverStationSim::NotifyNewData();

    EXPECT_TRUE(m_encoderSim.GetInitialized());
  }

  {
    wpi::sim::StepTiming(3_s);

    // Ensure arm is still at minimum angle.
    EXPECT_NEAR(kMinAngle.value(), m_encoderSim.GetDistance(), 2.0);
  }

  {
    // Press button to reach setpoint
    m_joystickSim.SetTrigger(true);
    m_joystickSim.NotifyNewData();

    wpi::sim::StepTiming(1.5_s);

    EXPECT_NEAR(setpoint.value(),
                wpi::units::radian_t(m_encoderSim.GetDistance())
                    .convert<wpi::units::degree>()
                    .value(),
                2.0);

    // see setpoint is held.
    wpi::sim::StepTiming(0.5_s);

    EXPECT_NEAR(setpoint.value(),
                wpi::units::radian_t(m_encoderSim.GetDistance())
                    .convert<wpi::units::degree>()
                    .value(),
                2.0);
  }

  {
    // Unpress the button to go back down
    m_joystickSim.SetTrigger(false);
    m_joystickSim.NotifyNewData();

    wpi::sim::StepTiming(3_s);

    EXPECT_NEAR(kMinAngle.value(), m_encoderSim.GetDistance(), 2.0);
  }

  {
    // Press button to go back up
    m_joystickSim.SetTrigger(true);
    m_joystickSim.NotifyNewData();

    // advance 75 timesteps
    wpi::sim::StepTiming(1.5_s);

    EXPECT_NEAR(setpoint.value(),
                wpi::units::radian_t(m_encoderSim.GetDistance())
                    .convert<wpi::units::degree>()
                    .value(),
                2.0);

    // advance 25 timesteps to see setpoint is held.
    wpi::sim::StepTiming(0.5_s);

    EXPECT_NEAR(setpoint.value(),
                wpi::units::radian_t(m_encoderSim.GetDistance())
                    .convert<wpi::units::degree>()
                    .value(),
                2.0);
  }

  {
    // Disable
    wpi::sim::DriverStationSim::SetAutonomous(false);
    wpi::sim::DriverStationSim::SetEnabled(false);
    wpi::sim::DriverStationSim::NotifyNewData();

    wpi::sim::StepTiming(3_s);

    ASSERT_NEAR(0.0, m_motorSim.GetSpeed(), 0.05);
    EXPECT_NEAR(kMinAngle.value(), m_encoderSim.GetDistance(), 2.0);
  }
}

INSTANTIATE_TEST_SUITE_P(
    ArmSimulationTests, ArmSimulationTest,
    testing::Values(kDefaultArmSetpoint, 25.0_deg, 50.0_deg),
    [](const testing::TestParamInfo<wpi::units::degree_t>& info) {
      return testing::PrintToString(info.param.value())
          .append(std::string(info.param.abbreviation()));
    });
