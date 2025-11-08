// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/driverstation/Joystick.hpp"
#include "wpi/romi/OnBoardIO.hpp"
#include "wpi/smartdashboard/SendableChooser.hpp"
#include "wpi/commands2/Command.hpp"
#include "wpi/commands2/CommandPtr.hpp"
#include "wpi/commands2/button/Trigger.hpp"

#include "Constants.hpp"
#include "commands/AutonomousDistance.hpp"
#include "commands/AutonomousTime.hpp"
#include "subsystems/Drivetrain.hpp"

/**
 * This class is where the bulk of the robot should be declared.  Since
 * Command-based is a "declarative" paradigm, very little robot logic should
 * actually be handled in the {@link Robot} periodic methods (other than the
 * scheduler calls).  Instead, the structure of the robot (including subsystems,
 * commands, and button mappings) should be declared here.
 */
class RobotContainer {
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
 public:
  RobotContainer();
  frc2::Command* GetAutonomousCommand();

 private:
  // Assumes a gamepad plugged into channel 0
  frc::Joystick m_controller{0};
  frc::SendableChooser<frc2::Command*> m_chooser;

  // The robot's subsystems
  Drivetrain m_drive;
  frc::OnBoardIO m_onboardIO{frc::OnBoardIO::ChannelMode::INPUT,
                             frc::OnBoardIO::ChannelMode::INPUT};

  // Example button
  frc2::Trigger m_onboardButtonA{
      [this] { return m_onboardIO.GetButtonAPressed(); }};

  // Autonomous commands.
  AutonomousDistance m_autoDistance{&m_drive};
  AutonomousTime m_autoTime{&m_drive};

  void ConfigureButtonBindings();
};
