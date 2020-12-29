// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.smartdashboard.SendableRegistry;

/**
 * REV Robotics SPARK Speed Controller.
 *
 * <p>Note that the SPARK uses the following bounds for PWM values. These values should work
 * reasonably well for most controllers, but if users experience issues such as asymmetric
 * behavior around the deadband or inability to saturate the controller in either direction,
 * calibration is recommended. The calibration procedure can be found in the Spark User Manual
 * available from REV Robotics.
 *
 * <p><ul>
 * <li>2.003ms = full "forward"
 * <li>1.550ms = the "high end" of the deadband range
 * <li>1.500ms = center of the deadband range (off)
 * <li>1.460ms = the "low end" of the deadband range
 * <li>0.999ms = full "reverse"
 * </ul>
 */
public class Spark extends PWMSpeedController {
  /**
   * Common initialization code called by all constructors.
   */
  protected void initSpark() {
    setBounds(2.003, 1.55, 1.50, 1.46, 0.999);
    setPeriodMultiplier(PeriodMultiplier.k1X);
    setSpeed(0.0);
    setZeroLatch();

    HAL.report(tResourceType.kResourceType_RevSPARK, getChannel() + 1);
    SendableRegistry.setName(this, "Spark", getChannel());
  }

  /**
   * Constructor.
   *
   * @param channel The PWM channel that the SPARK is attached to. 0-9 are on-board, 10-19 are on
   *                the MXP port
   */
  public Spark(final int channel) {
    super(channel);
    initSpark();
  }
}
