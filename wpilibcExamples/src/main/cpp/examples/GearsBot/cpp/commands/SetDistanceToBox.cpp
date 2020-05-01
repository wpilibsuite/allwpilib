/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "commands/SetDistanceToBox.h"

#include <frc/controller/PIDController.h>

#include "Robot.h"

SetDistanceToBox::SetDistanceToBox(double distance, DriveTrain* drivetrain)
    : frc2::CommandHelper<frc2::PIDCommand, SetDistanceToBox>(
          frc2::PIDController(-2, 0, 0),
          [this]() { return m_drivetrain->GetDistanceToObstacle(); }, distance,
          [this](double output) { m_drivetrain->Drive(output, output); },
          {drivetrain}),
      m_drivetrain(drivetrain) {
  m_controller.SetTolerance(0.01);
}

// Called just before this Command runs the first time
void SetDistanceToBox::Initialize() {
  // Get everything in a safe starting state.
  m_drivetrain->Reset();
  frc2::PIDCommand::Initialize();
}

bool SetDistanceToBox::IsFinished() { return m_controller.AtSetpoint(); }
