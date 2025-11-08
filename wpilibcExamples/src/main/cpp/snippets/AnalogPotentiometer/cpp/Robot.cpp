// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <wpi/framework/TimedRobot.hpp>
#include <wpi/hardware/discrete/AnalogInput.hpp>
#include <wpi/hardware/rotation/AnalogPotentiometer.hpp>

/**
 * AnalogPotentiometer snippets for frc-docs.
 * https://docs.wpilib.org/en/stable/docs/software/hardware-apis/sensors/analog-potentiometers-software.html
 */
class Robot : public wpi::TimedRobot {
 public:
  Robot() {
    // Set averaging bits to 2
    m_input.SetAverageBits(2);
  }

  void TeleopPeriodic() override {
    // Get the value of the potentiometer
    m_pot.Get();
  }

 private:
  // Initializes an AnalogPotentiometer on analog port 0
  // The full range of motion (in meaningful external units) is 0-180 (this
  // could be degrees, for instance) The "starting point" of the motion, i.e.
  // where the mechanism is located when the potentiometer reads 0v, is 30.
  wpi::AnalogPotentiometer m_pot{0, 180, 30};

  // Initializes an AnalogInput on port 1
  wpi::AnalogInput m_input{1};
  // Initializes an AnalogPotentiometer with the given AnalogInput
  // The full range of motion (in meaningful external units) is 0-180 (this
  // could be degrees, for instance) The "starting point" of the motion, i.e.
  // where the mechanism is located when the potentiometer reads 0v, is 30.
  wpi::AnalogPotentiometer m_pot1{&m_input, 180, 30};
};

#ifndef RUNNING_WPILIB_TESTS
int main() {
  return wpi::StartRobot<Robot>();
}
#endif
