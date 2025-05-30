// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc/AnalogInput.h>
#include <frc/TimedRobot.h>

/**
 * AnalogInput snippets for frc-docs.
 * https://docs.wpilib.org/en/stable/docs/software/hardware-apis/sensors/analog-input-software.html
 */
class Robot : public frc::TimedRobot {
 public:
  Robot() {
    // Sets the AnalogInput to 4-bit oversampling.  16 samples will be added
    // together.
    // Thus, the reported values will increase by about a factor of 16, and the
    // update rate will decrease by a similar amount.
    m_analog.SetOversampleBits(4);

    // Sets the AnalogInput to 4-bit averaging.  16 samples will be averaged
    // together. The update rate will decrease by a factor of 16.
    m_analog.SetAverageBits(4);

    // Gets the raw instantaneous measured value from the analog input, without
    // applying any calibration and ignoring oversampling and averaging
    // settings.
    m_analog.GetValue();

    // Gets the instantaneous measured voltage from the analog input.
    // Oversampling and averaging settings are ignored
    m_analog.GetVoltage();

    // Gets the averaged value from the analog input.  The value is not
    // rescaled, but oversampling and averaging are both applied.
    m_analog.GetAverageValue();

    // Gets the averaged voltage from the analog input.  Rescaling,
    // oversampling, and averaging are all applied.
    m_analog.GetAverageVoltage();
  }

  void TeleopPeriodic() override {
  }

 private:
  // Initializes an AnalogInput on port 0
  frc::AnalogInput m_analog{0};

  // The count and value variables to fill
  int64_t count;
  int64_t value;
};

#ifndef RUNNING_FRC_TESTS
int main() {
  return frc::StartRobot<Robot>();
}
#endif
