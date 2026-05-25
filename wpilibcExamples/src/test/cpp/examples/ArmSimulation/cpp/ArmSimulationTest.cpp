// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <string>
#include <thread>

#include <gtest/gtest.h>

#include "Constants.hpp"
#include "Robot.hpp"
#include "wpi/hal/DriverStationTypes.h"
#include "wpi/simulation/DriverStationSim.hpp"
#include "wpi/simulation/JoystickSim.hpp"
#include "wpi/simulation/PWMMotorControllerSim.hpp"
#include "wpi/simulation/SimHooks.hpp"
#include "wpi/units/time.hpp"
#include "wpi/util/Preferences.hpp"

class ArmSimulationTest : public testing::TestWithParam<wpi::units::degree_t> {
  Robot robot;
  std::optional<std::thread> thread;

 protected:
  wpi::sim::PWMMotorControllerSim motorSim{kMotorPort};
  wpi::sim::EncoderSim encoderSim =
      wpi::sim::EncoderSim::CreateForChannel(kEncoderAChannel);
  wpi::sim::JoystickSim joystickSim{kJoystickPort};

 public:
  void SetUp() override {
    wpi::sim::PauseTiming();
    wpi::sim::SetProgramStarted(false);

    thread = std::thread([&] { robot.StartCompetition(); });
    wpi::sim::WaitForProgramStart();
  }

  void TearDown() override {
    robot.EndCompetition();
    thread->join();

    encoderSim.ResetData();
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
    wpi::sim::DriverStationSim::SetRobotMode(HAL_ROBOT_MODE_TELEOPERATED);
    wpi::sim::DriverStationSim::SetEnabled(true);
    wpi::sim::DriverStationSim::NotifyNewData();

    EXPECT_TRUE(encoderSim.GetInitialized());
  }

  {
    wpi::sim::StepTiming(3_s);

    // Ensure arm is still at minimum angle.
    EXPECT_NEAR(kMinAngle.value(), encoderSim.GetDistance(), 2.0);
  }

  {
    // Press button to reach setpoint
    joystickSim.SetTrigger(true);
    joystickSim.NotifyNewData();

    wpi::sim::StepTiming(1.5_s);

    EXPECT_NEAR(setpoint.value(),
                wpi::units::radian_t(encoderSim.GetDistance())
                    .convert<wpi::units::degree>()
                    .value(),
                2.0);

    // see setpoint is held.
    wpi::sim::StepTiming(0.5_s);

    EXPECT_NEAR(setpoint.value(),
                wpi::units::radian_t(encoderSim.GetDistance())
                    .convert<wpi::units::degree>()
                    .value(),
                2.0);
  }

  {
    // Unpress the button to go back down
    joystickSim.SetTrigger(false);
    joystickSim.NotifyNewData();

    wpi::sim::StepTiming(3_s);

    EXPECT_NEAR(kMinAngle.value(), encoderSim.GetDistance(), 2.0);
  }

  {
    // Press button to go back up
    joystickSim.SetTrigger(true);
    joystickSim.NotifyNewData();

    // advance 75 timesteps
    wpi::sim::StepTiming(1.5_s);

    EXPECT_NEAR(setpoint.value(),
                wpi::units::radian_t(encoderSim.GetDistance())
                    .convert<wpi::units::degree>()
                    .value(),
                2.0);

    // advance 25 timesteps to see setpoint is held.
    wpi::sim::StepTiming(0.5_s);

    EXPECT_NEAR(setpoint.value(),
                wpi::units::radian_t(encoderSim.GetDistance())
                    .convert<wpi::units::degree>()
                    .value(),
                2.0);
  }

  {
    // Disable
    wpi::sim::DriverStationSim::SetEnabled(false);
    wpi::sim::DriverStationSim::NotifyNewData();

    wpi::sim::StepTiming(3_s);

    ASSERT_NEAR(0.0, motorSim.GetThrottle(), 0.05);
    EXPECT_NEAR(kMinAngle.value(), encoderSim.GetDistance(), 2.0);
  }
}

INSTANTIATE_TEST_SUITE_P(
    ArmSimulationTests, ArmSimulationTest,
    testing::Values(kDefaultArmSetpoint, 25.0_deg, 50.0_deg),
    [](const testing::TestParamInfo<wpi::units::degree_t>& info) {
      return testing::PrintToString(info.param.value())
          .append(std::string(info.param.abbreviation()));
    });
