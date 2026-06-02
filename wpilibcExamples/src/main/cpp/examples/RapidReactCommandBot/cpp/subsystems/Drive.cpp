// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "subsystems/Drive.hpp"

#include <utility>

#include "wpi/commands2/Commands.hpp"
#include "wpi/system/RobotController.hpp"

Drive::Drive() {
  wpi::util::SendableRegistry::AddChild(&drive, &leftLeader);
  wpi::util::SendableRegistry::AddChild(&drive, &rightLeader);

  leftLeader.AddFollower(leftFollower);
  rightLeader.AddFollower(rightFollower);

  // We need to invert one side of the drivetrain so that positive voltages
  // result in both sides moving forward. Depending on how your robot's
  // gearbox is constructed, you might have to invert the left side instead.
  rightLeader.SetInverted(true);

  // Sets the distance per pulse for the encoders
  leftEncoder.SetDistancePerPulse(DriveConstants::kEncoderDistancePerPulse);
  rightEncoder.SetDistancePerPulse(DriveConstants::kEncoderDistancePerPulse);

  // Set the controller to be continuous (because it is an angle controller)
  controller.EnableContinuousInput(-180_deg, 180_deg);
  // Set the controller tolerance - the delta tolerance ensures the robot is
  // stationary at the setpoint before it is considered as having reached the
  // reference
  controller.SetTolerance(DriveConstants::kTurnTolerance,
                          DriveConstants::kTurnRateTolerance);
}

wpi::cmd::CommandPtr Drive::ArcadeDriveCommand(std::function<double()> fwd,
                                               std::function<double()> rot) {
  return Run([this, fwd = std::move(fwd), rot = std::move(rot)] {
           drive.ArcadeDrive(fwd(), rot());
         })
      .WithName("ArcadeDrive");
}

wpi::cmd::CommandPtr Drive::DriveDistanceCommand(wpi::units::meter_t distance,
                                                 double velocity) {
  return RunOnce([this] {
           // Reset encoders at the start of the command
           leftEncoder.Reset();
           rightEncoder.Reset();
         })
      // Drive forward at specified velocity
      .AndThen(Run([this, velocity] { drive.ArcadeDrive(velocity, 0.0); }))
      .Until([this, distance] {
        return wpi::units::math::max(
                   wpi::units::meter_t(leftEncoder.GetDistance()),
                   wpi::units::meter_t(rightEncoder.GetDistance())) >= distance;
      })
      // Stop the drive when the command ends
      .FinallyDo([this](bool interrupted) { drive.StopMotor(); });
}

wpi::cmd::CommandPtr Drive::TurnToAngleCommand(wpi::units::degree_t angle) {
  return StartRun([this] { controller.Reset(imu.GetRotation2d().Degrees()); },
                  [this, angle] {
                    drive.ArcadeDrive(
                        0, controller.Calculate(imu.GetRotation2d().Degrees(),
                                                angle) +
                               // Divide feedforward voltage by battery voltage
                               // to normalize it to [-1, 1]
                               feedforward.Calculate(
                                   controller.GetSetpoint().velocity) /
                                   wpi::RobotController::GetBatteryVoltage());
                  })
      .Until([this] { return controller.AtGoal(); })
      .FinallyDo([this] { drive.ArcadeDrive(0, 0); });
}
