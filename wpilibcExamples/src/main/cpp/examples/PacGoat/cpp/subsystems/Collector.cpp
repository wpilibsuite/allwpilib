// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "subsystems/Collector.h"

#include <frc/livewindow/LiveWindow.h>

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
