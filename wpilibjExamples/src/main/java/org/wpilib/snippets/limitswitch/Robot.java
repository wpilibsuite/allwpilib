// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.snippets.limitswitch;

import org.wpilib.driverstation.Joystick;
import org.wpilib.framework.TimedRobot;
import org.wpilib.hardware.discrete.DigitalInput;
import org.wpilib.hardware.motor.PWMVictorSPX;

/**
 * Limit Switch snippets for frc-docs.
 * https://docs.wpilib.org/en/stable/docs/software/hardware-apis/sensors/limit-switch.html
 */
public class Robot extends TimedRobot {
  DigitalInput toplimitSwitch = new DigitalInput(0);
  DigitalInput bottomlimitSwitch = new DigitalInput(1);
  PWMVictorSPX motor = new PWMVictorSPX(0);
  Joystick joystick = new Joystick(0);

  @Override
  public void teleopPeriodic() {
    setMotorVelocity(joystick.getRawAxis(2));
  }

  /**
   * Sets the motor velocity based on joystick input while respecting limit switches.
   *
   * @param velocity the desired velocity of the motor, positive for up and negative for down
   */
  public void setMotorVelocity(double velocity) {
    if (velocity > 0) {
      if (toplimitSwitch.get()) {
        // We are going up and top limit is tripped so stop
        motor.setThrottle(0);
      } else {
        // We are going up but top limit is not tripped so go at commanded velocity
        motor.setThrottle(velocity);
      }
    } else {
      if (bottomlimitSwitch.get()) {
        // We are going down and bottom limit is tripped so stop
        motor.setThrottle(0);
      } else {
        // We are going down but bottom limit is not tripped so go at commanded velocity
        motor.setThrottle(velocity);
      }
    }
  }
}
