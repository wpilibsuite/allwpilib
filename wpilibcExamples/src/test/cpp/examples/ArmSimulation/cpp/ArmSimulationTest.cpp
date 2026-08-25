// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cmath>
#include <optional>
#include <thread>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "Constants.hpp"
#include "Robot.hpp"
#include "wpi/hal/DriverStationTypes.h"
#include "wpi/preferences/Preferences.hpp"
#include "wpi/simulation/DriverStationSim.hpp"
#include "wpi/simulation/JoystickSim.hpp"
#include "wpi/simulation/PWMMotorControllerSim.hpp"
#include "wpi/simulation/SimHooks.hpp"
#include "wpi/units/time.hpp"

class ArmSimulationTest {
  Robot robot;
  std::optional<std::thread> thread;

 public:
  wpi::sim::PWMMotorControllerSim motorSim{MOTOR_PORT};
  wpi::sim::EncoderSim encoderSim =
      wpi::sim::EncoderSim::CreateForChannel(ENCODER_A_CHANNEL);
  wpi::sim::JoystickSim joystickSim{JOYSTICK_PORT};

  ArmSimulationTest() {
    wpi::sim::PauseTiming();
    wpi::sim::SetProgramStarted(false);

    thread = std::thread([&] { robot.StartCompetition(); });
    wpi::sim::WaitForProgramStart();
  }

  ~ArmSimulationTest() {
    robot.EndCompetition();
    thread->join();

    encoderSim.ResetData();
    wpi::sim::DriverStationSim::ResetData();
    wpi::Preferences::RemoveAll();
  }
};

TEST_CASE_METHOD(ArmSimulationTest, "ArmSimulationTest teleop",
                 "[wpilibcExamples][examples][simulation][arm]") {
  wpi::units::degrees<> setpoint =
      GENERATE(DEFAULT_ARM_SETPOINT, 25.0_deg, 50.0_deg);

  CHECK(wpi::Preferences::ContainsKey(ARM_POSITION_KEY));
  CHECK(wpi::Preferences::ContainsKey(ARM_P_KEY));
  wpi::Preferences::SetDouble(ARM_POSITION_KEY, setpoint.value());
  CHECK_THAT(wpi::Preferences::GetDouble(ARM_POSITION_KEY, NAN),
             Catch::Matchers::WithinULP(setpoint.value(), 4));

  // teleop init
  {
    wpi::sim::DriverStationSim::SetRobotMode(wpi::hal::RobotMode::TELEOPERATED);
    wpi::sim::DriverStationSim::SetEnabled(true);
    wpi::sim::DriverStationSim::NotifyNewData();

    CHECK(encoderSim.GetInitialized());
  }

  {
    wpi::sim::StepTiming(3_s);

    // Ensure arm is still at minimum angle.
    CHECK_THAT(encoderSim.GetDistance(),
               Catch::Matchers::WithinAbs(MIN_ANGLE.value(), 2.0));
  }

  {
    // Press button to reach setpoint
    joystickSim.SetTrigger(true);
    joystickSim.NotifyNewData();

    wpi::sim::StepTiming(1.5_s);

    CHECK_THAT(wpi::units::radians<>{encoderSim.GetDistance()}
                   .convert<wpi::units::degrees_>()
                   .value(),
               Catch::Matchers::WithinAbs(setpoint.value(), 2.0));

    // see setpoint is held.
    wpi::sim::StepTiming(0.5_s);

    CHECK_THAT(wpi::units::radians<>{encoderSim.GetDistance()}
                   .convert<wpi::units::degrees_>()
                   .value(),
               Catch::Matchers::WithinAbs(setpoint.value(), 2.0));
  }

  {
    // Unpress the button to go back down
    joystickSim.SetTrigger(false);
    joystickSim.NotifyNewData();

    wpi::sim::StepTiming(3_s);

    CHECK_THAT(encoderSim.GetDistance(),
               Catch::Matchers::WithinAbs(MIN_ANGLE.value(), 2.0));
  }

  {
    // Press button to go back up
    joystickSim.SetTrigger(true);
    joystickSim.NotifyNewData();

    // advance 75 timesteps
    wpi::sim::StepTiming(1.5_s);

    CHECK_THAT(wpi::units::radians<>{encoderSim.GetDistance()}
                   .convert<wpi::units::degrees_>()
                   .value(),
               Catch::Matchers::WithinAbs(setpoint.value(), 2.0));

    // advance 25 timesteps to see setpoint is held.
    wpi::sim::StepTiming(0.5_s);

    CHECK_THAT(wpi::units::radians<>{encoderSim.GetDistance()}
                   .convert<wpi::units::degrees_>()
                   .value(),
               Catch::Matchers::WithinAbs(setpoint.value(), 2.0));
  }

  {
    // Disable
    wpi::sim::DriverStationSim::SetEnabled(false);
    wpi::sim::DriverStationSim::NotifyNewData();

    wpi::sim::StepTiming(3_s);

    REQUIRE_THAT(motorSim.GetThrottle(), Catch::Matchers::WithinAbs(0.0, 0.05));
    CHECK_THAT(encoderSim.GetDistance(),
               Catch::Matchers::WithinAbs(MIN_ANGLE.value(), 2.0));
  }
}
