// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <wpi/hardware/accelerometer/AnalogAccelerometer.hpp>
#include <wpi/hardware/discrete/AnalogInput.hpp>
#include <wpi/opmode/TimedRobot.hpp>

/**
 * AnalogAccelerometer snippets for frc-docs.
 * https://docs.wpilib.org/en/stable/docs/software/hardware-apis/sensors/accelerometers-software.html
 */
class Robot : public wpi::TimedRobot {
 public:
  Robot() {
    // Sets the sensitivity of the accelerometer to 1 volt per G
    m_accelerometer.SetSensitivity(1);
    // Sets the zero voltage of the accelerometer to 3 volts
    m_accelerometer.SetZero(3);
  }

  void TeleopPeriodic() override {
    // Gets the current acceleration
    m_accelerometer.GetAcceleration();
  }

 private:
  // Creates an analog accelerometer on analog input 0
  wpi::AnalogAccelerometer m_accelerometer{0};
};

#ifndef RUNNING_FRC_TESTS
int main() {
  return wpi::StartRobot<Robot>();
}
#endif
