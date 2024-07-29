// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/controller/ArmFeedforward.h>
#include <frc2/command/Commands.h>
#include <frc2/command/TrapezoidProfileSubsystem.h>
#include <units/angle.h>

#include "ExampleSmartMotorController.h"

/**
 * A robot arm subsystem that moves with a motion profile.
 */
class ArmSubsystem : public frc2::TrapezoidProfileSubsystem<units::radians> {
  using State = frc::TrapezoidProfile<units::radians>::State;

 public:
  ArmSubsystem();

  void UseState(State setpoint) override;

  frc2::CommandPtr SetArmGoalCommand(units::radian_t goal);

 private:
  ExampleSmartMotorController m_motor;
  frc::ArmFeedforward m_feedforward;
};
