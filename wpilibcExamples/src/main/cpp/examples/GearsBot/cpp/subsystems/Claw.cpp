/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "subsystems/Claw.h"

#include <frc/smartdashboard/SmartDashboard.h>

Claw::Claw() {
  // Let's show everything on the LiveWindow
  SetName("Claw");
  AddChild("Motor", &m_motor);
}

void Claw::Open() { m_motor.Set(-1); }

void Claw::Close() { m_motor.Set(1); }

void Claw::Stop() { m_motor.Set(0); }

bool Claw::IsGripping() { return m_contact.Get(); }

void Claw::Log() {
  frc::SmartDashboard::PutBoolean("Claw switch", IsGripping());
}

void Claw::Periodic() { Log(); }
