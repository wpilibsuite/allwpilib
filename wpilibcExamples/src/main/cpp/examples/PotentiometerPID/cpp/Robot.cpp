// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Robot.h"

void Robot::TeleopInit() {
  // Move to the bottom setpoint when teleop starts
  m_index = 0;
  m_pidController.SetSetpoint(SETPOINTS[m_index].value());
}

void Robot::TeleopPeriodic() {
  // Read from the sensor
  units::meter_t position = units::meter_t{m_potentiometer.Get()};

  // Run the PID Controller
  double pidOut = m_pidController.Calculate(position.value());

  // Apply PID output
  m_elevatorMotor.Set(pidOut);

  // when the button is pressed once, the selected elevator setpoint is
  // incremented
  if (m_joystick.GetTriggerPressed()) {
    // index of the elevator setpoint wraps around.
    m_index = (m_index + 1) % SETPOINTS.size();
    m_pidController.SetSetpoint(SETPOINTS[m_index].value());
  }
}

#ifndef RUNNING_FRC_TESTS
int main() {
  return frc::StartRobot<Robot>();
}
#endif
