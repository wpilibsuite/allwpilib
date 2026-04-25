// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/framework/TimedRobot.hpp"
#include "wpi/hardware/accelerometer/ADXL345_I2C.hpp"
#include "wpi/hardware/bus/I2C.hpp"

/**
 * ADXL346, 362 Accelerometer snippets for frc-docs.
 * https://docs.wpilib.org/en/stable/docs/software/hardware-apis/sensors/accelerometers-software.html
 */
class Robot : public wpi::TimedRobot {
 public:
  Robot() {}

  void TeleopPeriodic() override {
    // Gets the current acceleration in the X axis
    accelerometer.GetX();
    // Gets the current acceleration in the Y axis
    accelerometer.GetY();
    // Gets the current acceleration in the Z axis
    accelerometer.GetZ();
  }

 private:
  // Creates an ADXL345 accelerometer object with a measurement range from -8 to
  // 8 G's
  wpi::ADXL345_I2C accelerometer{wpi::I2C::Port::PORT_0, 8};
};

#ifndef RUNNING_WPILIB_TESTS
int main() {
  return wpi::StartRobot<Robot>();
}
#endif
