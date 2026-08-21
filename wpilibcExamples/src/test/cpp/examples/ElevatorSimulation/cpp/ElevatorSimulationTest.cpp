// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <optional>
#include <thread>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

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

class ElevatorSimulationTest {
  Robot robot;
  std::optional<std::thread> thread;

 public:
  wpi::sim::PWMMotorControllerSim motorSim{Constants::MOTOR_PORT};
  wpi::sim::EncoderSim encoderSim =
      wpi::sim::EncoderSim::CreateForChannel(Constants::ENCODER_A_CHANNEL);
  wpi::sim::JoystickSim joystickSim{Constants::JOYSTICK_PORT};

  ElevatorSimulationTest() {
    wpi::sim::PauseTiming();
    wpi::sim::SetProgramStarted(false);

    thread = std::thread([&] { robot.StartCompetition(); });
    wpi::sim::WaitForProgramStart();
  }

  ~ElevatorSimulationTest() {
    robot.EndCompetition();
    thread->join();

    encoderSim.ResetData();
    wpi::sim::DriverStationSim::ResetData();
  }
};

TEST_CASE_METHOD(ElevatorSimulationTest, "ElevatorSimulationTest teleop",
                 "[wpilibcExamples][examples][simulation][elevator]") {
  // teleop init
  {
    wpi::sim::DriverStationSim::SetRobotMode(wpi::hal::RobotMode::TELEOPERATED);
    wpi::sim::DriverStationSim::SetEnabled(true);
    wpi::sim::DriverStationSim::NotifyNewData();

    CHECK(encoderSim.GetInitialized());
  }

  {
    // advance 50 timesteps
    wpi::sim::StepTiming(1_s);

    // Ensure elevator is still at 0.
    CHECK_THAT(encoderSim.GetDistance(), Catch::Matchers::WithinAbs(0.0, 0.05));
  }

  {
    // Press button to reach setpoint
    joystickSim.SetTrigger(true);
    joystickSim.NotifyNewData();

    // advance 75 timesteps
    wpi::sim::StepTiming(1.5_s);

    CHECK_THAT(encoderSim.GetDistance(),
               Catch::Matchers::WithinAbs(SETPOINT.value(), 0.05));

    // advance 25 timesteps to see setpoint is held.
    wpi::sim::StepTiming(0.5_s);

    CHECK_THAT(encoderSim.GetDistance(),
               Catch::Matchers::WithinAbs(SETPOINT.value(), 0.05));
  }

  {
    // Unpress the button to go back down
    joystickSim.SetTrigger(false);
    joystickSim.NotifyNewData();

    // advance 75 timesteps
    wpi::sim::StepTiming(1.5_s);

    CHECK_THAT(encoderSim.GetDistance(), Catch::Matchers::WithinAbs(0.0, 0.05));
  }

  {
    // Press button to go back up
    joystickSim.SetTrigger(true);
    joystickSim.NotifyNewData();

    // advance 75 timesteps
    wpi::sim::StepTiming(1.5_s);

    CHECK_THAT(encoderSim.GetDistance(),
               Catch::Matchers::WithinAbs(SETPOINT.value(), 0.05));

    // advance 25 timesteps to see setpoint is held.
    wpi::sim::StepTiming(0.5_s);

    CHECK_THAT(encoderSim.GetDistance(),
               Catch::Matchers::WithinAbs(SETPOINT.value(), 0.05));
  }

  {
    // Disable
    wpi::sim::DriverStationSim::SetEnabled(false);
    wpi::sim::DriverStationSim::NotifyNewData();

    // advance 75 timesteps
    wpi::sim::StepTiming(1.5_s);

    REQUIRE_THAT(motorSim.GetThrottle(), Catch::Matchers::WithinAbs(0.0, 0.05));
    REQUIRE_THAT(encoderSim.GetDistance(),
                 Catch::Matchers::WithinAbs(0.0, 0.05));
  }
}
