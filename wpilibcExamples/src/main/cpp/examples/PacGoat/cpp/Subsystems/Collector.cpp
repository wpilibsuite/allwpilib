/*----------------------------------------------------------------------------*/
/* Use and modify this template how you wish, it's public domain.             */
/*----------------------------------------------------------------------------*/

#include "Subsystems/Collector.h"

#include <LiveWindow/LiveWindow.h>

Collector::Collector() : frc::Subsystem("Collector") {
  // Put everything to the LiveWindow for testing.
  AddChild("Roller Motor", m_rollerMotor);
  AddChild("Ball Detector", m_ballDetector);
  AddChild("Claw Open Detector", m_openDetector);
  AddChild("Piston", m_piston);
}

bool Collector::HasBall() {
  return m_ballDetector.Get();  // TODO: prepend ! to reflect real robot
}

void Collector::SetSpeed(double speed) { m_rollerMotor.Set(-speed); }

void Collector::Stop() { m_rollerMotor.Set(0); }

bool Collector::IsOpen() {
  return m_openDetector.Get();  // TODO: prepend ! to reflect real robot
}

void Collector::Open() { m_piston.Set(true); }

void Collector::Close() { m_piston.Set(false); }

void Collector::InitDefaultCommand() {}
