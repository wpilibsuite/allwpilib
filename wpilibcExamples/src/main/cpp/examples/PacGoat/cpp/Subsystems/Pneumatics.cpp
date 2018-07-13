/*----------------------------------------------------------------------------*/
/* Use and modify this template how you wish, it's public domain.             */
/*----------------------------------------------------------------------------*/

#include "Subsystems/Pneumatics.h"

#include <SmartDashboard/SmartDashboard.h>

Pneumatics::Pneumatics() : frc::Subsystem("Pneumatics") {
  AddChild("Pressure Sensor", m_pressureSensor);
}

void Pneumatics::InitDefaultCommand() {
  // No default command
}

void Pneumatics::Start() {
#ifndef SIMULATION
  m_compressor.Start();
#endif
}

bool Pneumatics::IsPressurized() {
#ifndef SIMULATION
  return kMaxPressure <= m_pressureSensor.GetVoltage();
#else
  return true;  // NOTE: Simulation always has full pressure
#endif
}

void Pneumatics::WritePressure() {
  frc::SmartDashboard::PutNumber("Pressure", m_pressureSensor.GetVoltage());
}
