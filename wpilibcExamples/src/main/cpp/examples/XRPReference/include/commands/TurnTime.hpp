// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/Timer.h>
#include <frc2/command/Command.h>
#include <frc2/command/CommandHelper.h>
#include <units/time.h>

#include "subsystems/Drivetrain.h"

class TurnTime : public frc2::CommandHelper<frc2::Command, TurnTime> {
 public:
  /**
   * Creates a new TurnTime.
   *
   * @param speed The speed which the robot will turn. Negative is in reverse.
   * @param time How much time to turn
   * @param drive The drive subsystem on which this command will run
   */
  TurnTime(double speed, units::second_t time, Drivetrain* drive)
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
