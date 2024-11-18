// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc2/command/Command.h>
#include <frc2/command/CommandHelper.h>
#include <units/length.h>

#include "subsystems/Drivetrain.h"

class DriveDistance : public frc2::CommandHelper<frc2::Command, DriveDistance> {
 public:
  /**
   * Creates a new DriveDistance. This command will drive your your robot for a
   * desired distance at a desired speed.
   *
   * @param speed The speed at which the robot will drive
   * @param distance The distance the robot will drive
   * @param drive The drivetrain subsystem on which this command will run
   */
  DriveDistance(double speed, units::meter_t distance, Drivetrain* drive)
      : m_speed(speed), m_distance(distance), m_drive(drive) {
    AddRequirements(m_drive);
  }

  void Initialize() override;
  void Execute() override;
  void End(bool interrupted) override;
  bool IsFinished() override;

 private:
  double m_speed;
  units::meter_t m_distance;
  Drivetrain* m_drive;
};
