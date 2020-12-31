// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc2/command/CommandBase.h>
#include <frc2/command/CommandHelper.h>
#include <units/angle.h>
#include <units/length.h>

#include "subsystems/Drivetrain.h"

class TurnDegrees : public frc2::CommandHelper<frc2::CommandBase, TurnDegrees> {
 public:
  TurnDegrees(double speed, units::degree_t angle, Drivetrain* drive)
      : m_speed(speed), m_angle(angle), m_drive(drive) {
    AddRequirements({m_drive});
  }

  void Initialize() override;
  void Execute() override;
  void End(bool interrupted) override;
  bool IsFinished() override;

 private:
  double m_speed;
  units::degree_t m_angle;
  Drivetrain* m_drive;

  units::meter_t GetAverageTurningDistance();
};
