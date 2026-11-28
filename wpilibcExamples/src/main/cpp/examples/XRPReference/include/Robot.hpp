// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "Constants.hpp"
#include "commands/AutonomousDistance.hpp"
#include "commands/AutonomousTime.hpp"
#include "subsystems/Arm.hpp"
#include "subsystems/Drivetrain.hpp"
#include "wpi/commands2/Command.hpp"
#include "wpi/commands2/CommandPtr.hpp"
#include "wpi/commands2/button/Trigger.hpp"
#include "wpi/driverstation/Joystick.hpp"
#include "wpi/framework/TimedRobot.hpp"
#include "wpi/tunables/Selectable.hpp"
#include "wpi/xrp/XRPOnBoardIO.hpp"

// NOTE: The I/O pin functionality of the 5 exposed I/O pins depends on the
// hardware "overlay"
// that is specified when launching the wpilib-ws server on the Romi raspberry
// pi. By default, the following are available (listed in order from inside of
// the board to outside):
// - DIO 8 (mapped to Arduino pin 11, closest to the inside of the board)
// - Analog In 0 (mapped to Analog Channel 6 / Arduino Pin 4)
// - Analog In 1 (mapped to Analog Channel 2 / Arduino Pin 20)
// - PWM 2 (mapped to Arduino Pin 21)
// - PWM 3 (mapped to Arduino Pin 22)
//
// Your subsystem configuration should take the overlays into account
class Robot : public wpi::TimedRobot {
 public:
  Robot();
  void RobotPeriodic() override;
  void DisabledStart() override;
  void DisabledPeriodic() override;
  void AutonomousStart() override;
  void AutonomousPeriodic() override;
  void AutonomousEnd() override;
  void TeleopStart() override;
  void TeleopPeriodic() override;
  void UtilityPeriodic() override;

 private:
  void ConfigureButtonBindings();
  // Have it null by default so that if testing teleop it
  // doesn't have undefined behavior and potentially crash.
  wpi::cmd::Command* autonomousCommand = nullptr;

  // Assumes a gamepad plugged into channel 0
  wpi::Joystick controller{0};
  wpi::tunables::Selectable<wpi::cmd::Command*> chooser;

  // The robot's subsystems
  Drivetrain drive;
  Arm arm;
  wpi::xrp::XRPOnBoardIO onboardIO;

  // Example button
  wpi::cmd::Trigger userButton{
      [this] { return onboardIO.GetUserButtonPressed(); }};

  // Autonomous commands.
  AutonomousDistance autoDistance{&drive};
  AutonomousTime autoTime{&drive};
};
