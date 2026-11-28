// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Robot.hpp"

#include "commands/DefaultDrive.hpp"
#include "commands/GrabHatch.hpp"
#include "commands/HalveDriveVelocity.hpp"
#include "commands/ReleaseHatch.hpp"
#include "wpi/commands2/CommandScheduler.hpp"
#include "wpi/commands2/button/GamepadButton.hpp"
#include "wpi/driverstation/DriverStation.hpp"
#include "wpi/system/DataLogManager.hpp"
#include "wpi/telemetry/Telemetry.hpp"
#include "wpi/tunables/Tunables.hpp"

Robot::Robot() {
  // Initialize all of your commands and subsystems here

  // Add commands to the autonomous command chooser
  chooser.AddDefault("Simple Auto", &simpleAuto);
  chooser.Add("Complex Auto", &complexAuto);

  // Put the chooser on the dashboard
  wpi::tunables::Publish("Autonomous", chooser);

  // Configure the button bindings
  ConfigureButtonBindings();

  // Set up default drive command
  drive.SetDefaultCommand(DefaultDrive(
      &drive, [this] { return -driverController.GetLeftY(); },
      [this] { return -driverController.GetRightX(); }));
  // Start recording to data log
  wpi::DataLogManager::Start();

  // Record DS control and joystick data.
  // Change to `false` to not record joystick data.
  wpi::DriverStation::StartDataLog(wpi::DataLogManager::GetLog(), true);
}

void Robot::ConfigureButtonBindings() {
  // Configure your button bindings here

  // NOTE: since we're binding a CommandPtr, command ownership here is moved to
  // the scheduler thus, no memory leaks!

  // Grab the hatch when the 'Face Down' button is pressed.
  wpi::cmd::GamepadButton(&driverController, wpi::Gamepad::Button::FACE_DOWN)
      .OnTrue(GrabHatch(&hatch).ToPtr());
  // Release the hatch when the 'Face Right' button is pressed.
  wpi::cmd::GamepadButton(&driverController, wpi::Gamepad::Button::FACE_RIGHT)
      .OnTrue(ReleaseHatch(&hatch).ToPtr());
  // While holding the bumper button, drive at half velocity
  wpi::cmd::GamepadButton(&driverController, wpi::Gamepad::Button::RIGHT_BUMPER)
      .WhileTrue(HalveDriveVelocity(&drive).ToPtr());
}

/**
 * This function is called every 20 ms, no matter the mode. Use
 * this for items like diagnostics that you want to run during disabled,
 * autonomous, teleoperated and utility.
 *
 * <p> This runs after the mode specific periodic functions, but before
 * LiveWindow and SmartDashboard integrated updating.
 */
void Robot::RobotPeriodic() {
  wpi::cmd::CommandScheduler::GetInstance().Run();
  // Put subsystems to dashboard.
  wpi::telemetry::Log("Drivetrain", drive);
  wpi::telemetry::Log("HatchSubsystem", hatch);
}

/**
 * This function is called each time Disabled mode starts. You
 * can use it to reset any subsystem information you want to clear when the
 * robot is disabled.
 */
void Robot::DisabledStart() {}

void Robot::DisabledPeriodic() {}

/**
 * This autonomous runs the autonomous command selected by your chooser.
 */
void Robot::AutonomousStart() {
  // Runs the chosen command in autonomous
  autonomousCommand = chooser.GetSelected();
  wpi::cmd::CommandScheduler::GetInstance().Schedule(autonomousCommand);
}

void Robot::AutonomousPeriodic() {}

void Robot::AutonomousEnd() {
  // This makes sure that the autonomous stops running when
  // autonomous mode ends. If you want the autonomous to
  // continue until interrupted by another command, remove
  // this line or comment it out.
  if (autonomousCommand != nullptr) {
    autonomousCommand->Cancel();
  }
}

void Robot::TeleopStart() {}

/**
 * This function is called periodically during teleoperated mode.
 */
void Robot::TeleopPeriodic() {}

/**
 * This function is called periodically during utility mode.
 */
void Robot::UtilityPeriodic() {}

#ifndef RUNNING_WPILIB_TESTS
int main() {
  return wpi::StartRobot<Robot>();
}
#endif
