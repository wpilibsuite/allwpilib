// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.servo;

/**
 * Actuonix L16-R servo.
 *
 * <p>This uses the following bounds for PWM values.
 * https://actuonix-com.3dcartstores.com/assets/images/datasheets/ActuonixL16Datasheet.pdf
 *
 * <ul>
 *   <li>2.0ms = full extend
 *   <li>1.0ms = full retract
 * </ul>
 */
public class ActuonixL16R extends LinearServo {
  public enum Stroke {
    _50mm(0.050),
    _100mm(0.100),
    _140mm(0.140);

    private double value;

    Stroke(double distanceMetres) {
      this.value = distanceMetres;
    }
  }

  /**
   * Constructor for a Actuonix L16-R servo.<br>
   *
   * @param channel The PWM channel to which the servo is attached. 0-9 are on-board, 10-19 are on
   *     the MXP port
   * @param stroke The stroke length of the actuonix servo (either 50, 100, or 140 mm).
   */
  public ActuonixL16R(final int channel, final Stroke stroke) {
    super("ActuonixL16R", channel, stroke.value);

    setBounds(2.0, 1.8, 1.5, 1.2, 1.0);
  }
}
