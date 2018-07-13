/*----------------------------------------------------------------------------*/
/* Use and modify this template how you wish, it's public domain.             */
/*----------------------------------------------------------------------------*/

#include "Subsystems/Wrist.h"

#include <SmartDashboard/SmartDashboard.h>

Wrist::Wrist() : frc::PIDSubsystem("Wrist", kP_real, 0.0, 0.0) {
#ifdef SIMULATION  // Check for simulation and update PID values
  GetPIDController()->SetPID(kP_simulation, 0, 0, 0);
#endif
  SetAbsoluteTolerance(2.5);

  // Let's show everything on the LiveWindow
  AddChild("Motor", m_motor);
  AddChild("Pot", m_pot);
}

void Wrist::InitDefaultCommand() {}

void Wrist::Log() {
  // frc::SmartDashboard::PutData("Wrist Angle", &m_pot);
}

double Wrist::ReturnPIDInput() { return m_pot.Get(); }

void Wrist::UsePIDOutput(double d) { m_motor.Set(d); }
