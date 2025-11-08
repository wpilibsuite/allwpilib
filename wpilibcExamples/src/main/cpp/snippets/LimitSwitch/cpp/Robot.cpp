// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hardware/discrete/DigitalInput.hpp"
#include "wpi/hardware/rotation/Encoder.hpp"
#include "wpi/driverstation/Joystick.hpp"
#include "wpi/opmode/TimedRobot.hpp"
#include "wpi/hardware/motor/PWMVictorSPX.hpp"

/**
 * Limit Switch snippets for frc-docs.
 * https://docs.wpilib.org/en/stable/docs/software/hardware-apis/sensors/limit-switch.html
 */
class Robot : public frc::TimedRobot {
 public:
  void TeleopPeriodic() override { SetMotorSpeed(m_joystick.GetRawAxis(2)); }
  void SetMotorSpeed(double speed) {
    if (speed > 0) {
      if (m_toplimitSwitch.Get()) {
        // We are going up and top limit is tripped so stop
        m_motor.Set(0);
      } else {
        // We are going up but top limit is not tripped so go at commanded speed
        m_motor.Set(speed);
      }
    } else {
      if (m_bottomlimitSwitch.Get()) {
        // We are going down and bottom limit is tripped so stop
        m_motor.Set(0);
      } else {
        // We are going down but bottom limit is not tripped so go at commanded
        // speed
        m_motor.Set(speed);
      }
    }
  }

 private:
  frc::DigitalInput m_toplimitSwitch{0};
  frc::DigitalInput m_bottomlimitSwitch{1};
  frc::PWMVictorSPX m_motor{0};
  frc::Joystick m_joystick{0};
};

#ifndef RUNNING_FRC_TESTS
int main() {
  return frc::StartRobot<Robot>();
}
#endif
