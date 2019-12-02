/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "subsystems/ArmSubsystem.h"

#include "Constants.h"

using namespace ArmConstants;
using State = frc::TrapezoidProfile<units::radians>::State;

ArmSubsystem::ArmSubsystem()
    : frc2::TrapezoidProfileSubsystem<units::radians>(
          {kMaxVelocity, kMaxAcceleration}, kArmOffset),
      m_motor(kMotorPort),
      m_feedforward(kS, kCos, kV, kA) {
  m_motor.SetPID(kP, 0, 0);
}

void ArmSubsystem::UseState(State setpoint) {
  // Calculate the feedforward from the sepoint
  units::volt_t feedforward =
      m_feedforward.Calculate(setpoint.position, setpoint.velocity);
  // Add the feedforward to the PID output to get the motor output
  m_motor.SetSetpoint(ExampleSmartMotorController::PIDMode::kPosition,
                      setpoint.position.to<double>(), feedforward / 12_V);
}
