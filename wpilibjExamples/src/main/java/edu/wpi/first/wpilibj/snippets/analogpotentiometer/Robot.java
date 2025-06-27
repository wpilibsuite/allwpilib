// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.snippets.analogpotentiometer;

import edu.wpi.first.wpilibj.AnalogInput;
import edu.wpi.first.wpilibj.AnalogPotentiometer;
import edu.wpi.first.wpilibj.TimedRobot;

/**
 * AnalogPotentiometer snippets for frc-docs.
 * https://docs.wpilib.org/en/stable/docs/software/hardware-apis/sensors/analog-poteniometers-software.html
 */
public class Robot extends TimedRobot {
  // Initializes an AnalogPotentiometer on analog port 0
  // The full range of motion (in meaningful external units) is 0-180 (this could be degrees, for
  // instance)
  // The "starting point" of the motion, i.e. where the mechanism is located when the potentiometer
  // reads 0v, is 30.
  AnalogPotentiometer m_pot = new AnalogPotentiometer(0, 180, 30);

  // Initializes an AnalogInput on port 1
  AnalogInput m_input = new AnalogInput(0);
  // Initializes an AnalogPotentiometer with the given AnalogInput
  // The full range of motion (in meaningful external units) is 0-180 (this could be degrees, for
  // instance)
  // The "starting point" of the motion, i.e. where the mechanism is located when the potentiometer
  // reads 0v, is 30.
  AnalogPotentiometer m_pot1 = new AnalogPotentiometer(m_input, 180, 30);

  /** Called once at the beginning of the robot program. */
  public Robot() {
    // Set averaging bits to 2
    m_input.setAverageBits(2);
  }

  @Override
  public void teleopPeriodic() {
    // Get the value of the potentiometer
    m_pot.get();

    m_pot1.get();
  }
}
