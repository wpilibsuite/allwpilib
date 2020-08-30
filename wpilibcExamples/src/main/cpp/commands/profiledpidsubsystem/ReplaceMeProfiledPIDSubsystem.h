/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <frc2/command/ProfiledPIDSubsystem.h>
#include <units/length.h>

class ReplaceMeProfiledPIDSubsystem
    : public frc2::ProfiledPIDSubsystem<units::meters> {
 public:
  ReplaceMeProfiledPIDSubsystem();

 protected:
  void UseOutput(double output,
                 frc::TrapezoidProfile<units::meters>::State setpoint) override;

  units::meter_t GetMeasurement() override;
};
