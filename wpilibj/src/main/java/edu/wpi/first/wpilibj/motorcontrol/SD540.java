// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.motorcontrol;

import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.PWM;

/**
 * Mindsensors SD540 Motor Controller.
 *
 * <p>Note that the SD540 uses the following bounds for PWM values. These values should work
 * reasonably well for most controllers, but if users experience issues such as asymmetric behavior
 * around the deadband or inability to saturate the controller in either direction, calibration is
 * recommended. The calibration procedure can be found in the SD540 User Manual available from
 * Mindsensors.
 *
 * <ul>
 *   <li>2.05ms = full "forward"
 *   <li>1.55ms = the "high end" of the deadband range
 *   <li>1.50ms = center of the deadband range (off)
 *   <li>1.44ms = the "low end" of the deadband range
 *   <li>0.94ms = full "reverse"
 * </ul>
 */
public class SD540 extends PWMMotorController {
  /**
   * Constructor.
   *
   * @param channel The PWM channel that the SD540 is attached to. 0-9 are on-board, 10-19 are on
   *     the MXP port
   */
  public SD540(final int channel) {
    super("SD540", channel);

    m_pwm.setBounds(2.05, 1.55, 1.50, 1.44, 0.94);
    m_pwm.setPeriodMultiplier(PWM.PeriodMultiplier.k1X);
    m_pwm.setSpeed(0.0);
    m_pwm.setZeroLatch();

    HAL.report(tResourceType.kResourceType_MindsensorsSD540, getChannel() + 1);
  }
}
