// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "subsystems/Drive.h"

#include <utility>

#include <frc2/command/Commands.h>

Drive::Drive() {
  // We need to invert one side of the drivetrain so that positive voltages
  // result in both sides moving forward. Depending on how your robot's
  // gearbox is constructed, you might have to invert the left side instead.
  m_rightMotors.SetInverted(true);

  // Sets the distance per pulse for the encoders
  m_leftEncoder.SetDistancePerPulse(DriveConstants::kEncoderDistancePerPulse);
  m_rightEncoder.SetDistancePerPulse(DriveConstants::kEncoderDistancePerPulse);
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
