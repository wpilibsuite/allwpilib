// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.smartdashboard.SendableRegistry;

/**
 * Playing with Fusion Venom Smart Motor with PWM control.
 *
 * <p>Note that the Venom uses the following bounds for PWM values. These values should work
 * reasonably well for most controllers, but if users experience issues such as asymmetric behavior
 * around the deadband or inability to saturate the controller in either direction, calibration is
 * recommended.
 *
 * <p>
 *
 * <ul>
 *   <li>2.004ms = full "forward"
 *   <li>1.520ms = the "high end" of the deadband range
 *   <li>1.500ms = center of the deadband range (off)
 *   <li>1.480ms = the "low end" of the deadband range
 *   <li>0.997ms = full "reverse"
 * </ul>
 */
public class PWMVenom extends PWMSpeedController {
  /**
   * Constructor for a Venom connected via PWM.
   *
   * @param channel The PWM channel that the Venom is attached to. 0-9 are on-board, 10-19 are on
   *     the MXP port
   */
  public PWMVenom(final int channel) {
    super(channel);

    setBounds(2.004, 1.52, 1.50, 1.48, 0.997);
    setPeriodMultiplier(PeriodMultiplier.k1X);
    setSpeed(0.0);
    setZeroLatch();

    HAL.report(tResourceType.kResourceType_FusionVenom, getChannel() + 1);
    SendableRegistry.setName(this, "PWMVenom", getChannel());
  }
}
