// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/Timer.h>
#include <frc2/command/Command.h>
#include <frc2/command/CommandHelper.h>
#include <units/time.h>

#include "subsystems/Drivetrain.h"

class DriveTime : public frc2::CommandHelper<frc2::Command, DriveTime> {
 public:
  /**
   * Creates a new DriveTime. This command will drive your robot for a desired
   * speed and time.
   *
   * @param speed The speed which the robot will drive. Negative is in reverse.
   * @param time How much time to drive
   * @param drive The drivetrain subsystem on which this command will run
   */
  DriveTime(double speed, units::second_t time, Drivetrain* drive)
      : m_speed(speed), m_duration(time), m_drive(drive) {
    AddRequirements(m_drive);
  }

  void Initialize() override;
  void Execute() override;
  void End(bool interrupted) override;
  bool IsFinished() override;

 private:
  double m_speed;
  units::second_t m_duration;
  Drivetrain* m_drive;
  frc::Timer m_timer;
};
