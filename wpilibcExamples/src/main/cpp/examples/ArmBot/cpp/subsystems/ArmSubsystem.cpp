/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "subsystems/ArmSubsystem.h"

#include "Constants.h"

using namespace ArmConstants;
using State = frc::TrapezoidProfile::State;

ArmSubsystem::ArmSubsystem()
    : frc2::ProfiledPIDSubsystem(frc::ProfiledPIDController(
          kP, 0, 0,
          {units::meters_per_second_t(kMaxVelocity.to<double>()),
           units::meters_per_second_squared_t(kMaxAcceleration.to<double>())})),
      m_motor(kMotorPort),
      m_encoder(kEncoderPorts[0], kEncoderPorts[1]),
      m_feedforward(kS, kCos, kV, kA),
      // Start arm at rest in neutral position
      m_goal{units::meter_t(kArmOffset.to<double>()), 0_mps} {
  m_encoder.SetDistancePerPulse(kEncoderDistancePerPulse.to<double>());
}

void ArmSubsystem::UseOutput(double output, State setpoint) {
  // Calculate the feedforward from the sepoint
  units::volt_t feedforward = m_feedforward.Calculate(
      units::radian_t(setpoint.position.to<double>()),
      units::radians_per_second_t(setpoint.velocity.to<double>()));
  // Add the feedforward to the PID output to get the motor output
  m_motor.SetVoltage(units::volt_t(output) + feedforward);
}

void ArmSubsystem::SetGoal(units::radian_t goal) {
  m_goal = State{units::meter_t(goal.to<double>()), 0_mps};
}

State ArmSubsystem::GetGoal() { return m_goal; }

units::meter_t ArmSubsystem::GetMeasurement() {
  return units::meter_t(m_encoder.GetDistance()) +
         units::meter_t(kArmOffset.to<double>());
}
