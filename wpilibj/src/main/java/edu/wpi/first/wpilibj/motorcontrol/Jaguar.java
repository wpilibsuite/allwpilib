// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.motorcontrol;

import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.PWM;

/**
 * Texas Instruments / Vex Robotics Jaguar Motor Controller as a PWM device.
 *
 * <p>Note that the Jaguar uses the following bounds for PWM values. These values should work
 * reasonably well for most controllers, but if users experience issues such as asymmetric behavior
 * around the deadband or inability to saturate the controller in either direction, calibration is
 * recommended. The calibration procedure can be found in the Jaguar User Manual available from Vex.
 *
 * <ul>
 *   <li>2.310ms = full "forward"
 *   <li>1.550ms = the "high end" of the deadband range
 *   <li>1.507ms = center of the deadband range (off)
 *   <li>1.454ms = the "low end" of the deadband range
 *   <li>0.697ms = full "reverse"
 * </ul>
 */
public class Jaguar extends PWMMotorController {
  /**
   * Constructor.
   *
   * @param channel The PWM channel that the Jaguar is attached to. 0-9 are on-board, 10-19 are on
   *     the MXP port
   */
  public Jaguar(final int channel) {
    super("Jaguar", channel);

    m_pwm.setBounds(2.31, 1.55, 1.507, 1.454, 0.697);
    m_pwm.setPeriodMultiplier(PWM.PeriodMultiplier.k1X);
    m_pwm.setSpeed(0.0);
    m_pwm.setZeroLatch();

    HAL.report(tResourceType.kResourceType_Jaguar, getChannel() + 1);
  }
}
