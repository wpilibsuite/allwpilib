// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <string>
#include <thread>

#include <frc/Preferences.h>
#include <frc/simulation/DriverStationSim.h>
#include <frc/simulation/JoystickSim.h>
#include <frc/simulation/PWMSim.h>
#include <frc/simulation/SimHooks.h>
#include <gtest/gtest.h>
#include <hal/simulation/MockHooks.h>
#include <units/length.h>
#include <units/time.h>

#include "Constants.h"
#include "Robot.h"

class ArmSimulationTest : public testing::TestWithParam<units::degree_t> {
  Robot m_robot;
  std::optional<std::thread> m_thread;

 protected:
  frc::sim::PWMSim m_motorSim{kMotorPort};
  frc::sim::EncoderSim m_encoderSim =
      frc::sim::EncoderSim::CreateForChannel(kEncoderAChannel);
  frc::sim::JoystickSim m_joystickSim{kJoystickPort};

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
    m_motorSim.ResetData();
    frc::sim::DriverStationSim::ResetData();
    frc::Preferences::RemoveAll();
  }
};

TEST_P(ArmSimulationTest, Teleop) {
  EXPECT_TRUE(frc::Preferences::ContainsKey(kArmPositionKey));
  EXPECT_TRUE(frc::Preferences::ContainsKey(kArmPKey));
  frc::Preferences::SetDouble(kArmPositionKey, GetParam().value());
  units::degree_t setpoint = GetParam();
  EXPECT_DOUBLE_EQ(setpoint.value(),
                   frc::Preferences::GetDouble(kArmPositionKey, NAN));

  // teleop init
  {
    frc::sim::DriverStationSim::SetAutonomous(false);
    frc::sim::DriverStationSim::SetEnabled(true);
    frc::sim::DriverStationSim::NotifyNewData();

    EXPECT_TRUE(m_motorSim.GetInitialized());
    EXPECT_TRUE(m_encoderSim.GetInitialized());
  }

  {
    frc::sim::StepTiming(3_s);

    // Ensure arm is still at minimum angle.
    EXPECT_NEAR(kMinAngle.value(), m_encoderSim.GetDistance(), 2.0);
  }

  {
    // Press button to reach setpoint
    m_joystickSim.SetTrigger(true);
    m_joystickSim.NotifyNewData();

    frc::sim::StepTiming(1.5_s);

    EXPECT_NEAR(setpoint.value(),
                units::radian_t(m_encoderSim.GetDistance())
                    .convert<units::degree>()
                    .value(),
                2.0);

    // see setpoint is held.
    frc::sim::StepTiming(0.5_s);

    EXPECT_NEAR(setpoint.value(),
                units::radian_t(m_encoderSim.GetDistance())
                    .convert<units::degree>()
                    .value(),
                2.0);
  }

  {
    // Unpress the button to go back down
    m_joystickSim.SetTrigger(false);
    m_joystickSim.NotifyNewData();

    frc::sim::StepTiming(3_s);

    EXPECT_NEAR(kMinAngle.value(), m_encoderSim.GetDistance(), 2.0);
  }

  {
    // Press button to go back up
    m_joystickSim.SetTrigger(true);
    m_joystickSim.NotifyNewData();

    // advance 75 timesteps
    frc::sim::StepTiming(1.5_s);

    EXPECT_NEAR(setpoint.value(),
                units::radian_t(m_encoderSim.GetDistance())
                    .convert<units::degree>()
                    .value(),
                2.0);

    // advance 25 timesteps to see setpoint is held.
    frc::sim::StepTiming(0.5_s);

    EXPECT_NEAR(setpoint.value(),
                units::radian_t(m_encoderSim.GetDistance())
                    .convert<units::degree>()
                    .value(),
                2.0);
  }

  {
    // Disable
    frc::sim::DriverStationSim::SetAutonomous(false);
    frc::sim::DriverStationSim::SetEnabled(false);
    frc::sim::DriverStationSim::NotifyNewData();

    frc::sim::StepTiming(3_s);

    ASSERT_NEAR(0.0, m_motorSim.GetSpeed(), 0.05);
    EXPECT_NEAR(kMinAngle.value(), m_encoderSim.GetDistance(), 2.0);
  }
}

INSTANTIATE_TEST_SUITE_P(
    ArmSimulationTests, ArmSimulationTest,
    testing::Values(kDefaultArmSetpoint, 25.0_deg, 50.0_deg),
    [](const testing::TestParamInfo<units::degree_t>& info) {
      return testing::PrintToString(info.param.value())
          .append(std::string(info.param.abbreviation()));
    });
