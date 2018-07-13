/*----------------------------------------------------------------------------*/
/* Use and modify this template how you wish, it's public domain.             */
/*----------------------------------------------------------------------------*/

#include "Subsystems/Claw.h"

Claw::Claw() : frc::Subsystem("Claw") {
  // Let's show everything on the LiveWindow
  AddChild("Motor", m_motor);
}

void Claw::InitDefaultCommand() {}

void Claw::Open() { m_motor.Set(-1); }

void Claw::Close() { m_motor.Set(1); }

void Claw::Stop() { m_motor.Set(0); }

bool Claw::IsGripping() { return m_contact.Get(); }

void Claw::Log() {}
