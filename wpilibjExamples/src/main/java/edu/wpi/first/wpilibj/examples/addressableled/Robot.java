/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.addressableled;

import edu.wpi.first.wpilibj.AddressableLED;
import edu.wpi.first.wpilibj.AddressableLEDBuffer;
import edu.wpi.first.wpilibj.TimedRobot;

public class Robot extends TimedRobot {
  private AddressableLED m_led;
  private AddressableLEDBuffer m_ledBuffer;
  private int m_count;

  @Override
  public void robotInit() {
    // PWM port 0
    // Must be a PWM header, not MXP or DIO
    m_led = new AddressableLED(0);

    // Default to a length of 12, start empty output
    // Length is expensive to set, so only set it once, then just update data
    m_led.setLength(12);

    // Reuse buffer
    m_ledBuffer = new AddressableLEDBuffer(12);

    m_led.setData(m_ledBuffer);
    m_led.start();

  }

  @Override
  public void robotPeriodic() {
    // Zero out all LEDS
    for (int i = 0; i < m_ledBuffer.getLength(); i++) {
      m_ledBuffer.setLED(i, 0, 0, 0);
    }

    // Set 1 single LED to red
    m_ledBuffer.setLED(m_count, 50, 0, 0);

    // Continue moving LED
    m_count++;
    if (m_count >= 12) {
      m_count = 0;
    }

    // Buffer must be written to update.
    m_led.setData(m_ledBuffer);
  }

}
