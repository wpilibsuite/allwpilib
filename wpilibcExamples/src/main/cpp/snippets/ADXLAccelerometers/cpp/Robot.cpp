// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <wpi/hardware/accelerometer/ADXL345_I2C.hpp>
#include <wpi/hardware/bus/I2C.hpp>
#include <wpi/hardware/discrete/AnalogInput.hpp>
#include <wpi/opmode/TimedRobot.hpp>

/**
 * ADXL346, 362 Accelerometer snippets for frc-docs.
 * https://docs.wpilib.org/en/stable/docs/software/hardware-apis/sensors/accelerometers-software.html
 */
class Robot : public wpi::TimedRobot {
 public:
  Robot() {}

  void TeleopPeriodic() override {
    // Gets the current acceleration in the X axis
    m_accelerometer.GetX();
    // Gets the current acceleration in the Y axis
    m_accelerometer.GetY();
    // Gets the current acceleration in the Z axis
    m_accelerometer.GetZ();
  }

 private:
  // Creates an ADXL345 accelerometer object on the MXP I2C port
  // with a measurement range from -8 to 8 G's
  wpi::ADXL345_I2C m_accelerometer{wpi::I2C::Port::kPort0,
                                   wpi::ADXL345_I2C::Range::kRange_8G};
};

#ifndef RUNNING_WPILIB_TESTS
int main() {
  return wpi::StartRobot<Robot>();
}
#endif
