/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <frc2/command/TrapezoidProfileSubsystem.h>
#include <units/length.h>

class ReplaceMeTrapezoidProfileSubsystem
    : frc2::TrapezoidProfileSubsystem<units::meters> {
 public:
  ReplaceMeTrapezoidProfileSubsystem();

 protected:
  void UseState(frc::TrapezoidProfile<units::meters>::State state) override;
};
