// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.motorcontrol;

import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.PWM;

/**
 * VEX Robotics Victor 888 Motor Controller The Vex Robotics Victor 884 Motor Controller can also be
 * used with this class but may need to be calibrated per the Victor 884 user manual.
 *
 * <p>Note that the Victor uses the following bounds for PWM values. These values were determined
 * empirically and optimized for the Victor 888. These values should work reasonably well for Victor
 * 884 controllers also but if users experience issues such as asymmetric behavior around the
 * deadband or inability to saturate the controller in either direction, calibration is recommended.
 * The calibration procedure can be found in the Victor 884 User Manual available from VEX Robotics:
 * http://content.vexrobotics.com/docs/ifi-v884-users-manual-9-25-06.pdf
 *
 * <ul>
 *   <li>2.027ms = full "forward"
 *   <li>1.525ms = the "high end" of the deadband range
 *   <li>1.507ms = center of the deadband range (off)
 *   <li>1.490ms = the "low end" of the deadband range
 *   <li>1.026ms = full "reverse"
 * </ul>
 */
public class Victor extends PWMMotorController {
  /**
   * Constructor.
   *
   * @param channel The PWM channel that the Victor is attached to. 0-9 are on-board, 10-19 are on
   *     the MXP port
   */
  public Victor(final int channel) {
    super("Victor", channel);

    m_pwm.setBounds(2.027, 1.525, 1.507, 1.49, 1.026);
    m_pwm.setPeriodMultiplier(PWM.PeriodMultiplier.k2X);
    m_pwm.setSpeed(0.0);
    m_pwm.setZeroLatch();

    HAL.report(tResourceType.kResourceType_Victor, getChannel() + 1);
  }
}
