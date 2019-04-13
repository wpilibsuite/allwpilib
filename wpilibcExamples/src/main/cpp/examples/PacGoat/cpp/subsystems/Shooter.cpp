/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "subsystems/Shooter.h"

Shooter::Shooter() : Subsystem("Shooter") {
  // Put everything to the LiveWindow for testing.
  AddChild("Hot Goal Sensor", m_hotGoalSensor);
  AddChild("Piston1 Reed Switch Front ", m_piston1ReedSwitchFront);
  AddChild("Piston1 Reed Switch Back ", m_piston1ReedSwitchBack);
  AddChild("Latch Piston", m_latchPiston);
}

void Shooter::InitDefaultCommand() {
  // Set the default command for a subsystem here.
  // SetDefaultCommand(new MySpecialCommand());
}

void Shooter::ExtendBoth() {
  m_piston1.Set(frc::DoubleSolenoid::kForward);
  m_piston2.Set(frc::DoubleSolenoid::kForward);
}

void Shooter::RetractBoth() {
  m_piston1.Set(frc::DoubleSolenoid::kReverse);
  m_piston2.Set(frc::DoubleSolenoid::kReverse);
}

void Shooter::Extend1() { m_piston1.Set(frc::DoubleSolenoid::kForward); }

void Shooter::Retract1() { m_piston1.Set(frc::DoubleSolenoid::kReverse); }

void Shooter::Extend2() { m_piston2.Set(frc::DoubleSolenoid::kReverse); }

void Shooter::Retract2() { m_piston2.Set(frc::DoubleSolenoid::kForward); }

void Shooter::Off1() { m_piston1.Set(frc::DoubleSolenoid::kOff); }

void Shooter::Off2() { m_piston2.Set(frc::DoubleSolenoid::kOff); }

void Shooter::Unlatch() { m_latchPiston.Set(true); }

void Shooter::Latch() { m_latchPiston.Set(false); }

void Shooter::ToggleLatchPosition() { m_latchPiston.Set(!m_latchPiston.Get()); }

bool Shooter::Piston1IsExtended() { return !m_piston1ReedSwitchFront.Get(); }

bool Shooter::Piston1IsRetracted() { return !m_piston1ReedSwitchBack.Get(); }

void Shooter::OffBoth() {
  m_piston1.Set(frc::DoubleSolenoid::kOff);
  m_piston2.Set(frc::DoubleSolenoid::kOff);
}

bool Shooter::GoalIsHot() { return m_hotGoalSensor.Get(); }
