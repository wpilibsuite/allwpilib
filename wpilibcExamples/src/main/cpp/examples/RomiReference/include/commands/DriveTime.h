// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/Timer.h>
#include <frc/command/CommandBase.h>
#include <frc/command/CommandHelper.h>
#include <units/time.h>

#include "subsystems/Drivetrain.h"

class DriveTime : public frc::CommandHelper<frc::CommandBase, DriveTime> {
 public:
  DriveTime(double speed, units::second_t time, Drivetrain* drive)
      : m_speed(speed), m_duration(time), m_drive(drive) {
    AddRequirements({m_drive});
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
