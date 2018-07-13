/*----------------------------------------------------------------------------*/
/* Use and modify this template how you wish, it's public domain.             */
/*----------------------------------------------------------------------------*/

#include "Subsystems/Elevator.h"

#include <LiveWindow/LiveWindow.h>
#include <SmartDashboard/SmartDashboard.h>

Elevator::Elevator() : frc::PIDSubsystem("Elevator", kP_real, kI_real, 0.0) {
#ifdef SIMULATION  // Check for simulation and update PID values
  GetPIDController()->SetPID(kP_simulation, kI_simulation, 0, 0);
#endif
  SetAbsoluteTolerance(0.005);

  // Let's show everything on the LiveWindow
  AddChild("Motor", m_motor);
  AddChild("Pot", &m_pot);
}

void Elevator::InitDefaultCommand() {}

void Elevator::Log() {
  // frc::SmartDashboard::PutData("Wrist Pot", &m_pot);
}

double Elevator::ReturnPIDInput() { return m_pot.Get(); }

void Elevator::UsePIDOutput(double d) { m_motor.Set(d); }
