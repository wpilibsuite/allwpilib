// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.snippets.limitswitch;

import org.wpilib.DigitalInput;
import org.wpilib.Joystick;
import org.wpilib.TimedRobot;
import org.wpilib.motorcontrol.PWMVictorSPX;

/**
 * Limit Switch snippets for frc-docs.
 * https://docs.wpilib.org/en/stable/docs/software/hardware-apis/sensors/limit-switch.html
 */
public class Robot extends TimedRobot {
  DigitalInput m_toplimitSwitch = new DigitalInput(0);
  DigitalInput m_bottomlimitSwitch = new DigitalInput(1);
  PWMVictorSPX m_motor = new PWMVictorSPX(0);
  Joystick m_joystick = new Joystick(0);

  @Override
  public void teleopPeriodic() {
    setMotorSpeed(m_joystick.getRawAxis(2));
  }

  /**
   * Sets the motor speed based on joystick input while respecting limit switches.
   *
   * @param speed the desired speed of the motor, positive for up and negative for down
   */
  public void setMotorSpeed(double speed) {
    if (speed > 0) {
      if (m_toplimitSwitch.get()) {
        // We are going up and top limit is tripped so stop
        m_motor.set(0);
      } else {
        // We are going up but top limit is not tripped so go at commanded speed
        m_motor.set(speed);
      }
    } else {
      if (m_bottomlimitSwitch.get()) {
        // We are going down and bottom limit is tripped so stop
        m_motor.set(0);
      } else {
        // We are going down but bottom limit is not tripped so go at commanded speed
        m_motor.set(speed);
      }
    }
  }
}
