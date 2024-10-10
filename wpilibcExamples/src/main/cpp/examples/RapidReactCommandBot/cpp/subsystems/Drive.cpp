// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "subsystems/Drive.h"

#include <utility>

#include <frc/RobotController.h>
#include <frc2/command/Commands.h>

Drive::Drive() {
  wpi::SendableRegistry::AddChild(&m_drive, &m_leftLeader);
  wpi::SendableRegistry::AddChild(&m_drive, &m_rightLeader);

  m_leftLeader.AddFollower(m_leftFollower);
  m_rightLeader.AddFollower(m_rightFollower);

  // We need to invert one side of the drivetrain so that positive voltages
  // result in both sides moving forward. Depending on how your robot's
  // gearbox is constructed, you might have to invert the left side instead.
  m_rightLeader.SetInverted(true);

  // Sets the distance per pulse for the encoders
  m_leftEncoder.SetDistancePerPulse(DriveConstants::kEncoderDistancePerPulse);
  m_rightEncoder.SetDistancePerPulse(DriveConstants::kEncoderDistancePerPulse);

  // Set the controller to be continuous (because it is an angle controller)
  m_controller.EnableContinuousInput(-180_deg, 180_deg);
  // Set the controller tolerance - the delta tolerance ensures the robot is
  // stationary at the setpoint before it is considered as having reached the
  // reference
  m_controller.SetTolerance(DriveConstants::kTurnTolerance,
                            DriveConstants::kTurnRateTolerance);
}

frc2::CommandPtr Drive::ArcadeDriveCommand(std::function<double()> fwd,
                                           std::function<double()> rot) {
  return Run([this, fwd = std::move(fwd), rot = std::move(rot)] {
           m_drive.ArcadeDrive(fwd(), rot());
         })
      .WithName("ArcadeDrive");
}

frc2::CommandPtr Drive::DriveDistanceCommand(units::meter_t distance,
                                             double speed) {
  return RunOnce([this] {
           // Reset encoders at the start of the command
           m_leftEncoder.Reset();
           m_rightEncoder.Reset();
         })
      // Drive forward at specified speed
      .AndThen(Run([this, speed] { m_drive.ArcadeDrive(speed, 0.0); }))
      .Until([this, distance] {
        return units::math::max(units::meter_t(m_leftEncoder.GetDistance()),
                                units::meter_t(m_rightEncoder.GetDistance())) >=
               distance;
      })
      // Stop the drive when the command ends
      .FinallyDo([this](bool interrupted) { m_drive.StopMotor(); });
}

frc2::CommandPtr Drive::TurnToAngleCommand(units::degree_t angle) {
  return StartRun(
             [this] { m_controller.Reset(m_gyro.GetRotation2d().Degrees()); },
             [this, angle] {
               m_drive.ArcadeDrive(
                   0, m_controller.Calculate(m_gyro.GetRotation2d().Degrees(),
                                             angle) +
                          // Divide feedforward voltage by battery voltage to
                          // normalize it to [-1, 1]
                          m_feedforward.Calculate(
                              m_controller.GetSetpoint().velocity) /
                              frc::RobotController::GetBatteryVoltage());
             })
      .Until([this] { return m_controller.AtGoal(); })
      .FinallyDo([this] { m_drive.ArcadeDrive(0, 0); });
}
