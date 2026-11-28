// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Robot.hpp"

#include "wpi/commands2/CommandScheduler.hpp"
#include "wpi/driverstation/DriverStation.hpp"
#include "wpi/system/DataLogManager.hpp"
#include "wpi/telemetry/Telemetry.hpp"
#include "wpi/tunables/Tunables.hpp"

Robot::Robot() {
  // Initialize all of your commands and subsystems here

  // Add commands to the autonomous command chooser
  // Note that we do *not* move ownership into the chooser
  chooser.AddDefault("Simple Auto", simpleAuto.get());
  chooser.Add("Complex Auto", complexAuto.get());

  // Put the chooser on the dashboard
  wpi::tunables::Publish("Autonomous", chooser);

  // Configure the button bindings
  ConfigureButtonBindings();

  // Set up default drive command
  drive.SetDefaultCommand(wpi::cmd::Run(
      [this] {
        drive.ArcadeDrive(-driverController.GetLeftY(),
                          -driverController.GetRightX());
      },
      {&drive}));
  // Start recording to data log
  wpi::DataLogManager::Start();

  // Record DS control and joystick data.
  // Change to `false` to not record joystick data.
  wpi::DriverStation::StartDataLog(wpi::DataLogManager::GetLog(), true);
}

void Robot::ConfigureButtonBindings() {
  // Configure your button bindings here

  // Grab the hatch when the 'Face Right' button is pressed.
  driverController.FaceRight().OnTrue(hatch.GrabHatchCommand());
  // Release the hatch when the 'Face Left' button is pressed.
  driverController.FaceLeft().OnTrue(hatch.ReleaseHatchCommand());
  // While holding Right Bumper, drive at half velocity
  driverController.RightBumper()
      .OnTrue(wpi::cmd::RunOnce([this] { drive.SetMaxOutput(0.5); }, {}))
      .OnFalse(wpi::cmd::RunOnce([this] { drive.SetMaxOutput(1.0); }, {}));
}
/**
 * This function is called every 20 ms, no matter the mode. Use
 * this for items like diagnostics that you want to run during disabled,
 * autonomous, teleoperated and utility.
 */
void Robot::RobotPeriodic() {
  wpi::cmd::CommandScheduler::GetInstance().Run();
  // Update the telemetry for all subsystems
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
