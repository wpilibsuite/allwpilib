// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc2/command/CommandBase.h>
#include <frc2/command/CommandHelper.h>
#include <units/length.h>

#include "subsystems/Drivetrain.h"

class DriveDistance
    : public frc2::CommandHelper<frc2::CommandBase, DriveDistance> {
 public:
  DriveDistance(double speed, units::meter_t distance, Drivetrain* drive)
      : m_speed(speed), m_distance(distance), m_drive(drive) {
    AddRequirements({m_drive});
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
