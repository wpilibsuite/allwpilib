/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc2/command/Subsystem.h"

using namespace frc2;
Subsystem::~Subsystem() {
  CommandScheduler::GetInstance().UnregisterSubsystem(this);
}

void Subsystem::Periodic() {}

void Subsystem::SimulationPeriodic() {}

Command* Subsystem::GetDefaultCommand() const {
  return CommandScheduler::GetInstance().GetDefaultCommand(this);
}

Command* Subsystem::GetCurrentCommand() const {
  return CommandScheduler::GetInstance().Requiring(this);
}

void Subsystem::Register() {
  return CommandScheduler::GetInstance().RegisterSubsystem(this);
}
