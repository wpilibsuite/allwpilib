// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "commands/DriveStraight.h"

#include <frc/controller/PIDController.h>

#include "Robot.h"

DriveStraight::DriveStraight(double distance, DriveTrain* drivetrain)
    : frc2::CommandHelper<frc2::PIDCommand, DriveStraight>(
          frc2::PIDController(4, 0, 0),
          [this]() { return m_drivetrain->GetDistance(); }, distance,
          [this](double output) { m_drivetrain->Drive(output, output); },
          {drivetrain}),
      m_drivetrain(drivetrain) {
  m_controller.SetTolerance(0.01);
}

// Called just before this Command runs the first time
void DriveStraight::Initialize() {
  // Get everything in a safe starting state.
  m_drivetrain->Reset();
  frc2::PIDCommand::Initialize();
}

bool DriveStraight::IsFinished() {
  return m_controller.AtSetpoint();
}
