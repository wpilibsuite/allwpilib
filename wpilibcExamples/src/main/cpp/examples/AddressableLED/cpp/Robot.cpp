/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <array>

#include <frc/AddressableLED.h>
#include <frc/TimedRobot.h>

class Robot : public frc::TimedRobot {
  // PWM port 0
  // Must be a PWM header, not MXP or DIO
  frc::AddressableLED m_led{0};
  std::array<frc::AddressableLED::LEDData, 12> m_ledBuffer;  // Reuse the buffer
  int m_count = 0;

 public:
  void RobotInit() override {
    // Default to a length of 12, start empty output
    // Length is expensive to set, so only set it once, then just update data
    m_led.SetLength(12);
    m_led.SetData(m_ledBuffer);
    m_led.Start();
  }

  void RobotPeriodic() override {
    // Zero out all LEDs
    for (auto& ledData : m_ledBuffer) {
      ledData.SetLED(0, 0, 0);
    }

    // Set 1 single LED to red
    m_ledBuffer[m_count].SetLED(50, 0, 0);

    // Continue moving LED
    m_count++;
    if (m_count >= 12) m_count = 0;

    // Buffer must be written to update.
    m_led.SetData(m_ledBuffer);
  }
};

#ifndef RUNNING_FRC_TESTS
int main() { return frc::StartRobot<Robot>(); }
#endif
