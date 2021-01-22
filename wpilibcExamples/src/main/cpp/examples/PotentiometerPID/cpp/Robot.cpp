// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <array>

#include <frc/AnalogInput.h>
#include <frc/Joystick.h>
#include <frc/PWMVictorSPX.h>
#include <frc/TimedRobot.h>
#include <frc/controller/PIDController.h>

/**
 * This is a sample program to demonstrate how to use a soft potentiometer and a
 * PID Controller to reach and maintain position setpoints on an elevator
 * mechanism.
 */
class Robot : public frc::TimedRobot {
 public:
  void RobotInit() override {
    m_pidController.SetSetpoint(kSetPoints[m_index]);
  }

  void TeleopPeriodic() override {
    // When the button is pressed once, the selected elevator setpoint is
    // incremented.
    bool currentButtonValue = m_joystick.GetTrigger();
    if (currentButtonValue && !m_previousButtonValue) {
      // Index of the elevator setpoint wraps around
      m_index = (m_index + 1) % (sizeof(kSetPoints) / 8);
      m_pidController.SetSetpoint(kSetPoints[m_index]);
    }
    m_previousButtonValue = currentButtonValue;

    double output =
        m_pidController.Calculate(m_potentiometer.GetAverageVoltage());
    m_elevatorMotor.Set(output);
  }

 private:
  static constexpr int kPotChannel = 1;
  static constexpr int kMotorChannel = 7;
  static constexpr int kJoystickChannel = 0;

  // Bottom, middle, and top elevator setpoints
  static constexpr std::array<double, 3> kSetPoints = {{1.0, 2.6, 4.3}};

  /* Proportional, integral, and derivative speed constants; motor inverted.
   *
   * DANGER: When tuning PID constants, high/inappropriate values for pGain,
   * iGain, and dGain may cause dangerous, uncontrollable, or undesired
   * behavior!
   *
   * These may need to be positive for a non-inverted motor.
   */
  static constexpr double kP = -5.0;
  static constexpr double kI = -0.02;
  static constexpr double kD = -2.0;

  int m_index = 0;
  bool m_previousButtonValue = false;

  frc::AnalogInput m_potentiometer{kPotChannel};
  frc::Joystick m_joystick{kJoystickChannel};
  frc::PWMVictorSPX m_elevatorMotor{kMotorChannel};

  frc2::PIDController m_pidController{kP, kI, kD};
};

constexpr std::array<double, 3> Robot::kSetPoints;

#ifndef RUNNING_FRC_TESTS
int main() {
  return frc::StartRobot<Robot>();
}
#endif
