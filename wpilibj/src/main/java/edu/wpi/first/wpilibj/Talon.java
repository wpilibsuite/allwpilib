/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.smartdashboard.SendableRegistry;

/**
 * Cross the Road Electronics (CTRE) Talon and Talon SR Speed Controller.
 *
 * <p>Note that the Talon uses the following bounds for PWM values. These values should work
 * reasonably well for most controllers, but if users experience issues such as asymmetric
 * behavior around the deadband or inability to saturate the controller in either direction,
 * calibration is recommended. The calibration procedure can be found in the Talon User Manual
 * available from CTRE.
 *
 * <p><ul>
 * <li>2.037ms = full "forward"
 * <li>1.539ms = the "high end" of the deadband range
 * <li>1.513ms = center of the deadband range (off)
 * <li>1.487ms = the "low end" of the deadband range
 * <li>0.989ms = full "reverse"
 * </ul>
 */
public class Talon extends PWMSpeedController {
  /**
   * Constructor for a Talon (original or Talon SR).
   *
   * @param channel The PWM channel that the Talon is attached to. 0-9 are on-board, 10-19 are on
   *                the MXP port
   */
  public Talon(final int channel) {
    super(channel);

    setBounds(2.037, 1.539, 1.513, 1.487, 0.989);
    setPeriodMultiplier(PeriodMultiplier.k1X);
    setSpeed(0.0);
    setZeroLatch();

    HAL.report(tResourceType.kResourceType_Talon, getChannel() + 1);
    SendableRegistry.setName(this, "Talon", getChannel());
  }
}
