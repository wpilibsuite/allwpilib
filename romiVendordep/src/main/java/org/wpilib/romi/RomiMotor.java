// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.romi;

import org.wpilib.hardware.discrete.PWM;
import org.wpilib.hardware.motor.PWMMotorController;

/**
 * RomiMotor.
 *
 * <p>A general use PWM motor controller representing the motors on a Romi robot
 */
public class RomiMotor extends PWMMotorController {
  /** Common initialization code called by all constructors. */
  protected final void initRomiMotor() {
    m_pwm.setOutputPeriod(PWM.OutputPeriod.k5Ms);
    setSpeed(0.0);
  }

  /**
   * Constructor.
   *
   * @param channel The PWM channel that the RomiMotor is attached to. 0 is the left motor, 1 is the
   *     right.
   */
  @SuppressWarnings("this-escape")
  public RomiMotor(final int channel) {
    super("Romi Motor", channel);
    initRomiMotor();
  }
}
