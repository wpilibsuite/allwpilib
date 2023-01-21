// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.servo;

import edu.wpi.first.math.util.Units;

/* Hitec HS-322HD servo provided in the FIRST Kit of Parts in 2008. */
public class HS322HD extends RotaryServo {
  /**
   * Constructor for a Hitec HS-322HD.<br>
   *
   * @param channel The PWM channel to which the servo is attached. 0-9 are on-board, 10-19 are on
   *     the MXP port
   */
  public HS322HD(final int channel) {
    super("HS322HD", channel, 0.0, Units.degreesToRadians(180.0));

    setBounds(2.4, 0, 0, 0, 0.6);
  }
}
