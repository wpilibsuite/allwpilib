// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.snippets.addressableled;

import static org.wpilib.units.Units.Meters;
import static org.wpilib.units.Units.MetersPerSecond;

import org.wpilib.framework.TimedRobot;
import org.wpilib.hardware.led.AddressableLED;
import org.wpilib.hardware.led.AddressableLEDBuffer;
import org.wpilib.hardware.led.LEDPattern;
import org.wpilib.units.measure.Distance;

public class Robot extends TimedRobot {
  private final AddressableLED led;
  private final AddressableLEDBuffer ledBuffer;

  // Create an LED pattern that will display a rainbow across
  // all hues at maximum saturation and half brightness
  private final LEDPattern rainbow = LEDPattern.rainbow(255, 128);

  // Our LED strip has a density of 120 LEDs per meter
  private static final Distance kLedSpacing = Meters.of(1 / 120.0);

  // Create a new pattern that scrolls the rainbow pattern across the LED strip, moving at a
  // velocity
  // of 1 meter per second.
  private final LEDPattern scrollingRainbow =
      rainbow.scrollAtAbsoluteVelocity(MetersPerSecond.of(1), kLedSpacing);

  /** Called once at the beginning of the robot program. */
  public Robot() {
    // SmartIO port 1
    led = new AddressableLED(1);

    // Reuse buffer
    // Default to a length of 60
    ledBuffer = new AddressableLEDBuffer(60);
    led.setLength(ledBuffer.getLength());

    // Set the data
    led.setData(ledBuffer);
  }

  @Override
  public void robotPeriodic() {
    // Update the buffer with the rainbow animation
    scrollingRainbow.applyTo(ledBuffer);
    // Set the LEDs
    led.setData(ledBuffer);
  }
}
