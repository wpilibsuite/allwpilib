// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc/ADXL345_I2C.h>
#include <frc/ADXL345_SPI.h>
#include <frc/ADXL362.h>
#include <frc/AnalogAccelerometer.h>
#include <frc/AnalogInput.h>
#include <frc/I2C.h>
#include <frc/SPI.h>
#include <frc/TimedRobot.h>

/**
 * ADXL346, 362 Accelerometer snippets for frc-docs.
 * https://docs.wpilib.org/en/stable/docs/software/hardware-apis/sensors/accelerometers-software.html
 */
class Robot : public frc::TimedRobot {
 public:
  Robot() {}

  void TeleopPeriodic() override {
    // Gets the current acceleration in the X axis
    m_accelerometer345I2C.GetX();
    // Gets the current acceleration in the Y axis
    m_accelerometer345I2C.GetY();
    // Gets the current acceleration in the Z axis
    m_accelerometer345I2C.GetZ();

    // Gets the current acceleration in the X axis
    m_accelerometer345SPI.GetX();
    // Gets the current acceleration in the Y axis
    m_accelerometer345SPI.GetY();
    // Gets the current acceleration in the Z axis
    m_accelerometer345SPI.GetZ();

    // Gets the current acceleration in the X axis
    m_accelerometer362.GetX();
    // Gets the current acceleration in the Y axis
    m_accelerometer362.GetY();
    // Gets the current acceleration in the Z axis
    m_accelerometer362.GetZ();
  }

 private:
  // Creates an ADXL345 accelerometer object on the MXP I2C port
  // with a measurement range from -8 to 8 G's
  frc::ADXL345_I2C m_accelerometer345I2C{frc::I2C::Port::kMXP,
                                         frc::ADXL345_I2C::Range::kRange_8G};

  // Creates an ADXL345 accelerometer object on the MXP SPI port
  // with a measurement range from -8 to 8 G's
  frc::ADXL345_SPI m_accelerometer345SPI{frc::SPI::Port::kMXP,
                                         frc::ADXL345_SPI::Range::kRange_8G};

  // Creates an ADXL362 accelerometer object on the MXP SPI port
  // with a measurement range from -8 to 8 G's
  frc::ADXL362 m_accelerometer362{frc::SPI::Port::kMXP,
                                  frc::ADXL362::Range::kRange_8G};
};

#ifndef RUNNING_FRC_TESTS
int main() {
  return frc::StartRobot<Robot>();
}
#endif
